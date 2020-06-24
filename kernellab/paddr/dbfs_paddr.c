#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <asm/pgtable.h>

#define MAX_LENGTH 200
#define BYTE 256

MODULE_LICENSE("GPL");

static struct dentry *dir, *output;
static struct task_struct *task;

unsigned long power(unsigned long base, int exp){
        unsigned long result = 1;
        int i;
        for(i = 0; i < exp; i++){ /* we assume that exp is positive integer */
                result *= base;
        }

        return result;
}

static ssize_t read_output(struct file *fp,
                        char __user *user_buffer,
                        size_t length,
                        loff_t *position)
{
        /* Implement read file operation */
        struct mm_struct *mm;  /* used for memory descriptor */

        /* We will use multi-level page table and do page-walk to find physical address */
        /* In our system, we use 5-level page table */
        pgd_t *pgd;
        p4d_t *p4d;
        pud_t *pud;
        pmd_t *pmd;
        pte_t *pte;

        unsigned long ppn; /* physical page number */
        unsigned long ppo; /* physical page offset */
        unsigned long paddr; /* physical address */
        unsigned long vaddr; /* virtual address */
        pid_t pid; /* pid of input file */

        /* Buffers used for exchanging data between user and kernel */
        unsigned char temp_buffer[MAX_LENGTH]; 
        unsigned char kernel_buffer[MAX_LENGTH];

        int i;

        /* Initialize buffer */
        memset(temp_buffer, 0, MAX_LENGTH);
        memset(kernel_buffer, 0, MAX_LENGTH);   
        if(copy_from_user(kernel_buffer, user_buffer, length)){ /* non-zero return: error */
                printk("Falied to copy from user buffer\n");
                return -1;
        }

        /* Read pid and vaddr from input file */
        /* In our system, pid maximum is 32768, thus we need only lower 2bytes */
        pid = (pid_t)(kernel_buffer[0] * power(BYTE, 0) + kernel_buffer[1] * power(BYTE, 1)); 

        /* In our system, we use 48 bit virtual address space, thus we need total 6bytes */
        vaddr = 0;
        for(i = 0; i < 6; i++){ 
                vaddr += kernel_buffer[8 + i] * power(BYTE, i);
        }

        /* Get task struct */
        if((task = pid_task(find_get_pid(pid), PIDTYPE_PID)) == NULL){
                printk("Invalid process\n");
                return -1;
        }

        mm = (task -> mm);

        /* Page table walk process */
        pgd = pgd_offset(mm, vaddr);
        if(pgd_none(*pgd)){
                printk("Failed to get pgd table\n");
                return -1;
        }

        p4d = p4d_offset(pgd, vaddr);
        if(p4d_none(*p4d)){
                printk("Failed to get p4d table\n");
                return -1;
        }

        pud = pud_offset(p4d, vaddr);
        if(pud_none(*pud)){
                printk("Failed to get pud table\n");
                return -1;
        }

        pmd = pmd_offset(pud, vaddr);
        if(pmd_none(*pmd)){
                printk("Failed to get pmd table\n");
                return -1;
        }

        pte = pte_offset_kernel(pmd, vaddr);
        if(pte_none(*pte)){
                printk("Failed to get pte table\n");
                return -1;
        }
        
        /* Get physical address using physical page number and physical page offset */
        ppn = pte_pfn(*pte); /* get physical page number using pte */
        ppo = vaddr & ~PAGE_MASK; /* offset is same for virtual and physical case */
        paddr = ppn * (PAGE_SIZE) + ppo; /* calculate physical address value */

        /* Write output */
        for(i = 0; i < 16; i++){
                temp_buffer[i] = kernel_buffer[i]; /* copy pid and vaddr first */
        } 

        for(i = 16; i < length; i++){
                temp_buffer[i] = (paddr % BYTE); /* write paddr one by one byte */
                paddr /= BYTE;
        }

        temp_buffer[length] = '\0'; /* null char */
        simple_read_from_buffer(user_buffer, length, position, temp_buffer, length); /* copy result to user buffer */
        return length;
}

static const struct file_operations dbfs_fops = {
        /* Mapping file operations with your functions */
        .read = read_output,
};

static int __init dbfs_module_init(void)
{
        /* Implement init module */
        if( (dir = debugfs_create_dir("paddr", NULL)) == 0 ) {
                printk("Paddr directory initialization failed\n");
                return -1;
        }

        if( (output = debugfs_create_file("output", S_IRUSR | S_IWUSR, dir, NULL, &dbfs_fops)) == 0 ){
                printk("Output file initialization failed\n");
                return -1;
        } 

#if 0
        dir = debugfs_create_dir("paddr", NULL);

        if (!dir) {
                printk("Cannot create paddr dir\n");
                return -1;
        }

        /* Fill in the arguments below */
        output = debugfs_create_file("output", , , , );
#endif

	printk("dbfs_paddr module initialize done\n");

        return 0;
}

static void __exit dbfs_module_exit(void)
{
        /* Implement exit module */
        debugfs_remove(dir);
	printk("dbfs_paddr module exit\n");
}

module_init(dbfs_module_init);
module_exit(dbfs_module_exit);
