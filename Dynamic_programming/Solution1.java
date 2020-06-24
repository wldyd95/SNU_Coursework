import java.util.PriorityQueue;
import java.util.StringTokenizer;
import java.io.FileReader;
import java.io.BufferedReader;
import java.io.PrintWriter;
import java.util.Hashtable;


/*
   1. 아래와 같은 명령어를 입력하면 컴파일이 이루어져야 하며, Solution1 라는 이름의 클래스가 생성되어야 채점이 이루어집니다.
       javac Solution1.java -encoding UTF8


   2. 컴파일 후 아래와 같은 명령어를 입력했을 때 여러분의 프로그램이 정상적으로 출력파일 output1.txt 를 생성시켜야 채점이 이루어집니다.
       java Solution1

   - 제출하시는 소스코드의 인코딩이 UTF8 이어야 함에 유의 바랍니다.
   - 수행시간 측정을 위해 다음과 같이 time 명령어를 사용할 수 있습니다.
       time java Solution1
   - 일정 시간 초과시 프로그램을 강제 종료 시키기 위해 다음과 같이 timeout 명령어를 사용할 수 있습니다.
       timeout 0.5 java Solution1   // 0.5초 수행
       timeout 1 java Solution1     // 1초 수행
 */

class Solution1 {
    static final int max_n = 1000000;
    static int[] A = new int[max_n];
    static int[] copied = new int[max_n];
    static int n;
    static int Answer1, Answer2, Answer3;
    static long start;
    static double time1, time2, time3;

    public static void main(String[] args) throws Exception {
		/*
		   동일 폴더 내의 input1.txt 로부터 데이터를 읽어옵니다.
		   또한 동일 폴더 내의 output1.txt 로 정답을 출력합니다.
		 */
        BufferedReader br = new BufferedReader(new FileReader("input1.txt"));
        StringTokenizer stk;
        PrintWriter pw = new PrintWriter("output1.txt");

		/*
		   10개의 테스트 케이스가 주어지므로, 각각을 처리합니다.
		 */
        for (int test_case = 1; test_case <= 10; test_case++) {
			/*
			   각 테스트 케이스를 표준 입력에서 읽어옵니다.
			   먼저 배열의 원소의 개수를 n에 읽어들입니다.
			   그리고 각 원소를 A[0], A[1], ... , A[n-1]에 읽어들입니다.
			 */
            stk = new StringTokenizer(br.readLine());
            n = Integer.parseInt(stk.nextToken());
            stk = new StringTokenizer(br.readLine());
            for (int i = 0; i < n; i++) {
                A[i] = Integer.parseInt(stk.nextToken());
            }

            /////////////////////////////////////////////////////////////////////////////////////////////
			/*
			   이 부분에서 여러분의 알고리즘이 수행됩니다. in-place 정렬을 고려하여,
               여러분이 구현한 각각의 함수에 복사한 배열을 입력으로 넣어야 합니다.
			   문제의 답을 계산하여 그 값을 Answer에 저장하는 것을 가정하였습니다.
               함수를 구현하면 Answer1, Answer2, Answer3에 해당하는 주석을 제거하고 제출하세요.

               문제 1은 java 프로그래밍 연습을 위한 과제입니다.
			 */

            /* Problem 1-1 */
            System.arraycopy(A, 0, copied, 0, n);
            start = System.currentTimeMillis();
            Answer1 = remainder_sum(mergeSort_original(copied, 0, n - 1));
            time1 = (System.currentTimeMillis() - start) / 1000.;

            /* Problem 1-2 */
            System.arraycopy(A, 0, copied, 0, n);
            start = System.currentTimeMillis();
            Answer2 = remainder_sum(mergeSort_16(copied, 0, n-1));
            time2 = (System.currentTimeMillis() - start) / 1000.;

            /* Problem 1-3 */
            System.arraycopy(A, 0, copied, 0, n);
            start = System.currentTimeMillis();
            Answer3 = remainder_sum(mergeSort_16_heap(copied, 0, n-1));
            time3 = (System.currentTimeMillis() - start) / 1000.;


            /* 여러분의 답안 Answer1, Answer2, Answer3을 비교하는 코드를 아래에 작성 */
            if(Answer1 == Answer2 && Answer2 == Answer3) System.out.println("Yes");
            else System.out.println("No");
            /////////////////////////////////////////////////////////////////////////////////////////////


            // output1.txt로 답안을 출력합니다. Answer1, Answer2, Answer3 중 구현된 함수의 답안 출력
            pw.println("#" + test_case + " " + Answer1 + " " + time1 + " " + time2 + " " + time3);
			/*
			   아래 코드를 수행하지 않으면 여러분의 프로그램이 제한 시간 초과로 강제 종료 되었을 때,
			   출력한 내용이 실제로 파일에 기록되지 않을 수 있습니다.
			   따라서 안전을 위해 반드시 flush() 를 수행하시기 바랍니다.
			 */
            pw.flush();
        }

        br.close();
        pw.close();
    }

    // Original merge sort (merge 2 sorted array recursively)
    // sort A[p] ... A[r] only

    ////////////////////////////////////////////////////////////////////////////////////

    // time complexity analysis of original merge sort
    // As we know in class, in original merge sort we get T(n) = 2 * T(n / 2) + O(n)
    // Θ(n) is from (n / 2) to (n - 1)
    // Therefore, time complexity of original merge sort is T(n) = O(n * log(n))

    ////////////////////////////////////////////////////////////////////////////////////

    public static int[] mergeSort_original(int[] A, int p, int r){
        int l = (r - p + 1);
        int[] sorted;

        if(l == 1) return A; // end of recursion : return only result

        // divide input array into 2 arrays
        int[] left = new int[l / 2];
        int[] right = new int[l - l / 2];

        // allocate each array value
        for(int i = 0; i < l / 2; i++){
            left[i] = A[i];
        }

        for(int i = 0; i < (l - l / 2); i++){
            right[i] = A[i + l / 2];
        }

        left = mergeSort_original(left, 0, left.length - 1);
        right = mergeSort_original(right, 0, right.length - 1);
        sorted = merge_original(left, right);
        return sorted;
    }

    // Merge method of original merge sort
    public static int[] merge_original(int[] left, int[] right){
        int l = left.length;
        int r = right.length;
        int[] merged = new int[l + r];

        // allocate index pin of sorted left and right array
        int il = 0; // left array index pin
        int ir = 0; // right array index pin
        int im = 0; // merged array index pin

        while(il < l){
            if(ir < r){
                if(left[il] < right[ir]) merged[im++] = left[il++];
                else merged[im++] = right[ir++];
            }

            // If right array does not exist and only left array survive
            else merged[im++] = left[il++];
        }

        // If left array does not exist and only right array survive
        while(ir < r) merged[im++] = right[ir++];
        return merged;
    }

    // Second method merge sort (merge 16 sorted array recursively)
    // sort A[p] ... A[r] only
    // when merging, use linear comparison method
    // when array length is less than 16, we will just use 2 merge sort method above

    ////////////////////////////////////////////////////////////////////////////////////

    // time complexity analysis of merge sort by dividing into 16 sub arrays
    // when merging, we well use linear comparison method
    // when array length is less than 16, we will just use 2 merge sort method above, this does not affect total time complexity
    // in merge process we need to compare 16 numbers every time and this demands 15 comparison
    // there are 16 sub arrays and each array size is n / 16
    // therefore, overhead of merge process is from 15 * (15 / 16) * n to 15 * ( n - 1 )
    // In conclusion, overhead of our algorithm is O(n) ( but constant factor is greater than original merge sort)
    // T(n) = 16 * T(n / 16) + O(n)
    // Therefore T(n) = O(n * log(n))

    ////////////////////////////////////////////////////////////////////////////////////


    public static int[] mergeSort_16(int[] A, int p, int r) {
        int l = (r - p + 1);
        int[] sorted;

        // if length of input array is less than 16, we will just use 2-way merge sort!
        if(l < 16) return mergeSort_original(A, p, r);

        // length of A is larger than 16 case
        // divide input array into 16 arrays
        int[] sub0 = new int[l / 16];
        int[] sub1 = new int[l / 16];
        int[] sub2 = new int[l / 16];
        int[] sub3 = new int[l / 16];
        int[] sub4 = new int[l / 16];
        int[] sub5 = new int[l / 16];
        int[] sub6 = new int[l / 16];
        int[] sub7 = new int[l / 16];
        int[] sub8 = new int[l / 16];
        int[] sub9 = new int[l / 16];
        int[] sub10 = new int[l / 16];
        int[] sub11 = new int[l / 16];
        int[] sub12 = new int[l / 16];
        int[] sub13 = new int[l / 16];
        int[] sub14 = new int[l / 16];
        int[] sub15 = new int[l - 15 * (l / 16)];

        // allocate each array value
        for(int i = 0; i < l / 16; i++){
            sub0[i] = A[i];
        }

        for(int i = 0; i < l / 16; i++){
            sub1[i] = A[i + l / 16];
        }

        for(int i = 0; i < l / 16; i++){
            sub2[i] = A[i + 2 * (l / 16)];
        }

        for(int i = 0; i < l / 16; i++){
            sub3[i] = A[i + 3 * (l / 16)];
        }

        for(int i = 0; i < l / 16; i++){
            sub4[i] = A[i + 4 * (l / 16)];
        }

        for(int i = 0; i < l / 16; i++){
            sub5[i] = A[i + 5 * (l / 16)];
        }

        for(int i = 0; i < l / 16; i++){
            sub6[i] = A[i + 6 * (l / 16)];
        }

        for(int i = 0; i < l / 16; i++){
            sub7[i] = A[i + 7 * (l / 16)];
        }

        for(int i = 0; i < l / 16; i++){
            sub8[i] = A[i + 8 * (l / 16)];
        }

        for(int i = 0; i < l / 16; i++){
            sub9[i] = A[i + 9 * (l / 16)];
        }

        for(int i = 0; i < l / 16; i++){
            sub10[i] = A[i + 10 * (l / 16)];
        }

        for(int i = 0; i < l / 16; i++){
            sub11[i] = A[i + 11 * (l / 16)];
        }

        for(int i = 0; i < l / 16; i++){
            sub12[i] = A[i + 12 * (l / 16)];
        }

        for(int i = 0; i < l / 16; i++){
            sub13[i] = A[i + 13 * (l / 16)];
        }

        for(int i = 0; i < l / 16; i++){
            sub14[i] = A[i + 14 * (l / 16)];
        }

        for(int i = 0; i < (l - 15 * (l / 16)); i++){
            sub15[i] = A[i + 15 * (l / 16)];
        }

        // sort each array recursively
        sub0 = mergeSort_16(sub0, 0, sub0.length - 1);
        sub1 = mergeSort_16(sub1, 0, sub1.length - 1);
        sub2 = mergeSort_16(sub2, 0, sub2.length - 1);
        sub3 = mergeSort_16(sub3, 0, sub3.length - 1);
        sub4 = mergeSort_16(sub4, 0, sub4.length - 1);
        sub5 = mergeSort_16(sub5, 0, sub5.length - 1);
        sub6 = mergeSort_16(sub6, 0, sub6.length - 1);
        sub7 = mergeSort_16(sub7, 0, sub7.length - 1);
        sub8 = mergeSort_16(sub8, 0, sub8.length - 1);
        sub9 = mergeSort_16(sub9, 0, sub9.length - 1);
        sub10 = mergeSort_16(sub10, 0, sub10.length - 1);
        sub11 = mergeSort_16(sub11, 0, sub11.length - 1);
        sub12 = mergeSort_16(sub12, 0, sub12.length - 1);
        sub13 = mergeSort_16(sub13, 0, sub13.length - 1);
        sub14 = mergeSort_16(sub14, 0, sub14.length - 1);
        sub15 = mergeSort_16(sub15, 0, sub15.length - 1);

        int[][] sorted_array_list = {sub0, sub1, sub2, sub3, sub4, sub5, sub6, sub7, sub8, sub9, sub10, sub11, sub12, sub13, sub14, sub15};
        sorted = merge_16(sorted_array_list);
        return sorted;
    }

    public static int[] merge_16(int[][] sorted_array_list){
        int infinity = (int)Double.POSITIVE_INFINITY; // used for large number when each array is empty

        int l0 = sorted_array_list[0].length;
        int l1 = sorted_array_list[1].length;
        int l2 = sorted_array_list[2].length;
        int l3 = sorted_array_list[3].length;
        int l4 = sorted_array_list[4].length;
        int l5 = sorted_array_list[5].length;
        int l6 = sorted_array_list[6].length;
        int l7 = sorted_array_list[7].length;
        int l8 = sorted_array_list[8].length;
        int l9 = sorted_array_list[9].length;
        int l10 = sorted_array_list[10].length;
        int l11 = sorted_array_list[11].length;
        int l12 = sorted_array_list[12].length;
        int l13 = sorted_array_list[13].length;
        int l14 = sorted_array_list[14].length;
        int l15 = sorted_array_list[15].length;

        int[] merged = new int[l0 + l1 + l2 + l3 + l4 + l5 + l6 + l7 + l8 + l9 + l10 + l11 + l12 + l13 + l14 + l15];

        // allocate index pin of each sorted array
        int i0 = 0;
        int i1 = 0;
        int i2 = 0;
        int i3 = 0;
        int i4 = 0;
        int i5 = 0;
        int i6 = 0;
        int i7 = 0;
        int i8 = 0;
        int i9 = 0;
        int i10 = 0;
        int i11 = 0;
        int i12 = 0;
        int i13 = 0;
        int i14 = 0;
        int i15 = 0;
        int im = 0; // merged array index pin

        int[] minimum_array = new int[16];
        for(int i = 0; i < 16; i++){
            minimum_array[i] = sorted_array_list[i][0];
        }

        // merge process
        while(i0 < l0 || i1 < l1 || i2 < l2 || i3 < l3 || i4 < l4 || i5 < l5 || i6 < l6 || i7 < l7 || i8 < l8 || i9 < l9 || i10 < l10 ||
              i11 < l11 || i12 < l12 || i13 < l13 || i14 < l14 || i15 < l15 ){

            // find minimum value in minimum array by linear method (not effective)
            int min = find_minimum(minimum_array)[0];
            int i_min =  find_minimum(minimum_array)[1];
            merged[im++] = min;

            switch (i_min){
                case 0:
                    i0++;
                    if(i0 == l0) minimum_array[0] = infinity;
                    else{
                        minimum_array[0] = sorted_array_list[0][i0];
                    }
                    break;

                case 1:
                    i1++;
                    if(i1 == l1) minimum_array[1] = infinity;
                    else{
                        minimum_array[1] = sorted_array_list[1][i1];
                    }
                    break;

                case 2:
                    i2++;
                    if(i2 == l2) minimum_array[2] = infinity;
                    else{
                        minimum_array[2] = sorted_array_list[2][i2];
                    }
                    break;

                case 3:
                    i3++;
                    if(i3 == l3) minimum_array[3] = infinity;
                    else{
                        minimum_array[3] = sorted_array_list[3][i3];
                    }
                    break;

                case 4:
                    i4++;
                    if(i4 == l4) minimum_array[4] = infinity;
                    else{
                        minimum_array[4] = sorted_array_list[4][i4];
                    }
                    break;

                case 5:
                    i5++;
                    if(i5 == l5) minimum_array[5] = infinity;
                    else{
                        minimum_array[5] = sorted_array_list[5][i5];
                    }
                    break;

                case 6:
                    i6++;
                    if(i6 == l6) minimum_array[6] = infinity;
                    else{
                        minimum_array[6] = sorted_array_list[6][i6];
                    }
                    break;

                case 7:
                    i7++;
                    if(i7 == l7) minimum_array[7] = infinity;
                    else{
                        minimum_array[7] = sorted_array_list[7][i7];
                    }
                    break;

                case 8:
                    i8++;
                    if(i8 == l8) minimum_array[8] = infinity;
                    else{
                        minimum_array[8] = sorted_array_list[8][i8];
                    }
                    break;

                case 9:
                    i9++;
                    if(i9 == l9) minimum_array[9] = infinity;
                    else{
                        minimum_array[9] = sorted_array_list[9][i9];
                    }
                    break;

                case 10:
                    i10++;
                    if(i10 >= l10) minimum_array[10] = infinity;
                    else{
                        minimum_array[10] = sorted_array_list[10][i10];
                    }
                    break;

                case 11:
                    i11++;
                    if(i11 == l11) minimum_array[11] = infinity;
                    else{
                        minimum_array[11] = sorted_array_list[11][i11];
                    }
                    break;

                case 12:
                    i12++;
                    if(i12 == l12) minimum_array[12] = infinity;
                    else{
                        minimum_array[12] = sorted_array_list[12][i12];
                    }
                    break;

                case 13:
                    i13++;
                    if(i13 == l13) minimum_array[13] = infinity;
                    else{
                        minimum_array[13] = sorted_array_list[13][i13];
                    }
                    break;

                case 14:
                    i14++;
                    if(i14 == l14) minimum_array[14] = infinity;
                    else{
                        minimum_array[14] = sorted_array_list[14][i14];
                    }
                    break;

                case 15:
                    i15++;
                    if(i15 == l15) minimum_array[15] = infinity;
                    else{
                        minimum_array[15] = sorted_array_list[15][i15];
                    }
                    break;
            }
        }

        return merged;

    }

    // Third method merge sort (merge 16 sorted array recursively)
    // sort A[p] ... A[r] only
    // when merging, use min heap method ( use java priority queue for min heap )
    // when array length is less than 16, we will just use 2 merge sort method above

    ////////////////////////////////////////////////////////////////////////////////////

    // time complexity analysis of merge sort by dividing into 16 sub arrays
    // when merging, use min heap method (use java priority queue for min heap)
    // then array length is less than 16, we will just use 2 merge sort method above, this does not affect total time complexity
    // we make min heap using minimum value in each sub arrays (there are 16 elements in min heap)
    // for making heap, overhead is O(log16)
    // above overhead applies to every element, then total overhead is O(n * log16) (smaller!)
    // T(n) = 16 * T(n / 16) + O(n * log16) = 16 * T(n / 16) + O(n)
    // Therefore, T(n) = O(n * log(n))

    ////////////////////////////////////////////////////////////////////////////////////


    public static int[] mergeSort_16_heap(int[] A, int p, int r){
        int l = (r - p + 1);
        int[] sorted;

        // if length of input array is less than 16, we will just use 2-way merge sort!
        if(l < 16) return mergeSort_original(A, p, r);

        // length of A is larger than 16 case
        // divide input array into 16 arrays
        int[] sub0 = new int[l / 16];
        int[] sub1 = new int[l / 16];
        int[] sub2 = new int[l / 16];
        int[] sub3 = new int[l / 16];
        int[] sub4 = new int[l / 16];
        int[] sub5 = new int[l / 16];
        int[] sub6 = new int[l / 16];
        int[] sub7 = new int[l / 16];
        int[] sub8 = new int[l / 16];
        int[] sub9 = new int[l / 16];
        int[] sub10 = new int[l / 16];
        int[] sub11 = new int[l / 16];
        int[] sub12 = new int[l / 16];
        int[] sub13 = new int[l / 16];
        int[] sub14 = new int[l / 16];
        int[] sub15 = new int[l - 15 * (l / 16)];

        // allocate each array value
        for(int i = 0; i < l / 16; i++){
            sub0[i] = A[i];
        }

        for(int i = 0; i < l / 16; i++){
            sub1[i] = A[i + l / 16];
        }

        for(int i = 0; i < l / 16; i++){
            sub2[i] = A[i + 2 * (l / 16)];
        }

        for(int i = 0; i < l / 16; i++){
            sub3[i] = A[i + 3 * (l / 16)];
        }

        for(int i = 0; i < l / 16; i++){
            sub4[i] = A[i + 4 * (l / 16)];
        }

        for(int i = 0; i < l / 16; i++){
            sub5[i] = A[i + 5 * (l / 16)];
        }

        for(int i = 0; i < l / 16; i++){
            sub6[i] = A[i + 6 * (l / 16)];
        }

        for(int i = 0; i < l / 16; i++){
            sub7[i] = A[i + 7 * (l / 16)];
        }

        for(int i = 0; i < l / 16; i++){
            sub8[i] = A[i + 8 * (l / 16)];
        }

        for(int i = 0; i < l / 16; i++){
            sub9[i] = A[i + 9 * (l / 16)];
        }

        for(int i = 0; i < l / 16; i++){
            sub10[i] = A[i + 10 * (l / 16)];
        }

        for(int i = 0; i < l / 16; i++){
            sub11[i] = A[i + 11 * (l / 16)];
        }

        for(int i = 0; i < l / 16; i++){
            sub12[i] = A[i + 12 * (l / 16)];
        }

        for(int i = 0; i < l / 16; i++){
            sub13[i] = A[i + 13 * (l / 16)];
        }

        for(int i = 0; i < l / 16; i++){
            sub14[i] = A[i + 14 * (l / 16)];
        }

        for(int i = 0; i < (l - 15 * (l / 16)); i++){
            sub15[i] = A[i + 15 * (l / 16)];
        }

        // sort each array recursively
        sub0 = mergeSort_16_heap(sub0, 0, sub0.length - 1);
        sub1 = mergeSort_16_heap(sub1, 0, sub1.length - 1);
        sub2 = mergeSort_16_heap(sub2, 0, sub2.length - 1);
        sub3 = mergeSort_16_heap(sub3, 0, sub3.length - 1);
        sub4 = mergeSort_16_heap(sub4, 0, sub4.length - 1);
        sub5 = mergeSort_16_heap(sub5, 0, sub5.length - 1);
        sub6 = mergeSort_16_heap(sub6, 0, sub6.length - 1);
        sub7 = mergeSort_16_heap(sub7, 0, sub7.length - 1);
        sub8 = mergeSort_16_heap(sub8, 0, sub8.length - 1);
        sub9 = mergeSort_16_heap(sub9, 0, sub9.length - 1);
        sub10 = mergeSort_16_heap(sub10, 0, sub10.length - 1);
        sub11 = mergeSort_16_heap(sub11, 0, sub11.length - 1);
        sub12 = mergeSort_16_heap(sub12, 0, sub12.length - 1);
        sub13 = mergeSort_16_heap(sub13, 0, sub13.length - 1);
        sub14 = mergeSort_16_heap(sub14, 0, sub14.length - 1);
        sub15 = mergeSort_16_heap(sub15, 0, sub15.length - 1);

        int[][] sorted_array_list = {sub0, sub1, sub2, sub3, sub4, sub5, sub6, sub7, sub8, sub9, sub10, sub11, sub12, sub13, sub14, sub15};
        sorted = merge_16_heap(sorted_array_list);
        return sorted;
    }

    public static int[] merge_16_heap(int[][] sorted_array_list) {
        int infinity = (int) Double.POSITIVE_INFINITY; // used for large number when each array is empty

        int l0 = sorted_array_list[0].length;
        int l1 = sorted_array_list[1].length;
        int l2 = sorted_array_list[2].length;
        int l3 = sorted_array_list[3].length;
        int l4 = sorted_array_list[4].length;
        int l5 = sorted_array_list[5].length;
        int l6 = sorted_array_list[6].length;
        int l7 = sorted_array_list[7].length;
        int l8 = sorted_array_list[8].length;
        int l9 = sorted_array_list[9].length;
        int l10 = sorted_array_list[10].length;
        int l11 = sorted_array_list[11].length;
        int l12 = sorted_array_list[12].length;
        int l13 = sorted_array_list[13].length;
        int l14 = sorted_array_list[14].length;
        int l15 = sorted_array_list[15].length;

        int[] merged = new int[l0 + l1 + l2 + l3 + l4 + l5 + l6 + l7 + l8 + l9 + l10 + l11 + l12 + l13 + l14 + l15];

        // allocate index pin of each sorted array
        int i0 = 0;
        int i1 = 0;
        int i2 = 0;
        int i3 = 0;
        int i4 = 0;
        int i5 = 0;
        int i6 = 0;
        int i7 = 0;
        int i8 = 0;
        int i9 = 0;
        int i10 = 0;
        int i11 = 0;
        int i12 = 0;
        int i13 = 0;
        int i14 = 0;
        int i15 = 0;
        int im = 0; // merged array index pin

        // make hash table for each sorted array
        // we have to make this hash table because we have to know which sorted_array_list each value is from
        // we can use this method because we assume every value is different
        Hashtable<Integer, Integer> table = new Hashtable<>(); // this table contains <value, sorted_array_list number>
        for (int i = 0; i < l0; i++) {
            table.put(sorted_array_list[0][i], 0);
        }

        for (int i = 0; i < l1; i++) {
            table.put(sorted_array_list[1][i], 1);
        }

        for (int i = 0; i < l2; i++) {
            table.put(sorted_array_list[2][i], 2);
        }

        for (int i = 0; i < l3; i++) {
            table.put(sorted_array_list[3][i], 3);
        }

        for (int i = 0; i < l4; i++) {
            table.put(sorted_array_list[4][i], 4);
        }

        for (int i = 0; i < l5; i++) {
            table.put(sorted_array_list[5][i], 5);
        }

        for (int i = 0; i < l6; i++) {
            table.put(sorted_array_list[6][i], 6);
        }

        for (int i = 0; i < l7; i++) {
            table.put(sorted_array_list[7][i], 7);
        }

        for (int i = 0; i < l8; i++) {
            table.put(sorted_array_list[8][i], 8);
        }

        for (int i = 0; i < l9; i++) {
            table.put(sorted_array_list[9][i], 9);
        }

        for (int i = 0; i < l10; i++) {
            table.put(sorted_array_list[10][i], 10);
        }

        for (int i = 0; i < l11; i++) {
            table.put(sorted_array_list[11][i], 11);
        }

        for (int i = 0; i < l12; i++) {
            table.put(sorted_array_list[12][i], 12);
        }

        for (int i = 0; i < l13; i++) {
            table.put(sorted_array_list[13][i], 13);
        }

        for (int i = 0; i < l14; i++) {
            table.put(sorted_array_list[14][i], 14);
        }

        for (int i = 0; i < l15; i++) {
            table.put(sorted_array_list[15][i], 15);
        }

        // make min heap using priority queue
        PriorityQueue<Integer> minimum_heap = new PriorityQueue<>();
        for (int i = 0; i < 16; i++) {
            minimum_heap.offer(sorted_array_list[i][0]);
        }

        // merge process
        while (i0 < l0 || i1 < l1 || i2 < l2 || i3 < l3 || i4 < l4 || i5 < l5 || i6 < l6 || i7 < l7 || i8 < l8 || i9 < l9 || i10 < l10 ||
                i11 < l11 || i12 < l12 || i13 < l13 || i14 < l14 || i15 < l15) {

            // find minimum value in minimum heap (more effective method)
            int min = minimum_heap.poll();
            int i_min = table.get(min); // fnd i_min by using hash table
            merged[im++] = min;

            switch (i_min) {
                case 0:
                    i0++;
                    if (i0 == l0) minimum_heap.offer(infinity);
                    else {
                        minimum_heap.offer(sorted_array_list[0][i0]);
                    }
                    break;

                case 1:
                    i1++;
                    if (i1 == l1) minimum_heap.offer(infinity);
                    else {
                        minimum_heap.offer(sorted_array_list[1][i1]);
                    }
                    break;

                case 2:
                    i2++;
                    if (i2 == l2) minimum_heap.offer(infinity);
                    else {
                        minimum_heap.offer(sorted_array_list[2][i2]);
                    }
                    break;

                case 3:
                    i3++;
                    if (i3 == l3) minimum_heap.offer(infinity);
                    else {
                        minimum_heap.offer(sorted_array_list[3][i3]);
                    }
                    break;

                case 4:
                    i4++;
                    if (i4 == l4) minimum_heap.offer(infinity);
                    else {
                        minimum_heap.offer(sorted_array_list[4][i4]);
                    }
                    break;

                case 5:
                    i5++;
                    if (i5 == l5) minimum_heap.offer(infinity);
                    else {
                        minimum_heap.offer(sorted_array_list[5][i5]);
                    }
                    break;

                case 6:
                    i6++;
                    if (i6 == l6) minimum_heap.offer(infinity);
                    else {
                        minimum_heap.offer(sorted_array_list[6][i6]);
                    }
                    break;

                case 7:
                    i7++;
                    if (i7 == l7) minimum_heap.offer(infinity);
                    else {
                        minimum_heap.offer(sorted_array_list[7][i7]);
                    }
                    break;

                case 8:
                    i8++;
                    if (i8 == l8) minimum_heap.offer(infinity);
                    else {
                        minimum_heap.offer(sorted_array_list[8][i8]);
                    }
                    break;

                case 9:
                    i9++;
                    if (i9 == l9) minimum_heap.offer(infinity);
                    else {
                        minimum_heap.offer(sorted_array_list[9][i9]);
                    }
                    break;

                case 10:
                    i10++;
                    if (i10 == l10) minimum_heap.offer(infinity);
                    else {
                        minimum_heap.offer(sorted_array_list[10][i10]);
                    }
                    break;

                case 11:
                    i11++;
                    if (i11 == l11) minimum_heap.offer(infinity);
                    else {
                        minimum_heap.offer(sorted_array_list[11][i11]);
                    }
                    break;

                case 12:
                    i12++;
                    if (i12 == l12) minimum_heap.offer(infinity);
                    else {
                        minimum_heap.offer(sorted_array_list[12][i12]);
                    }
                    break;

                case 13:
                    i13++;
                    if (i13 == l13) minimum_heap.offer(infinity);
                    else {
                        minimum_heap.offer(sorted_array_list[13][i13]);
                    }
                    break;

                case 14:
                    i14++;
                    if (i14 == l14) minimum_heap.offer(infinity);
                    else {
                        minimum_heap.offer(sorted_array_list[14][i14]);
                    }
                    break;

                case 15:
                    i15++;
                    if (i15 == l15) minimum_heap.offer(infinity);
                    else {
                        minimum_heap.offer(sorted_array_list[15][i15]);
                    }
                    break;
            }
        }

        return merged;
    }


    // return sum of remainder when we divide A[0], A[4], A[8] ... by 7
    public static int remainder_sum(int[] A){
        int l = A.length;
        int remainder_sum = 0;
        int remainder;
        int i = 0;

        while(i < l){
            remainder = A[i] - 7 * (A[i] / 7);
            remainder_sum += remainder;
            i += 4;
        }

        return remainder_sum;
    }

    // find minimum value in array by linear method
    // returns {minimum value, minimum index}}
    public static int[] find_minimum(int[] A){
        int minimum = A[0];
        int index_minium = 0;

        for(int i = 0; i < A.length; i++){
            if(A[i] < minimum){
                minimum = A[i];
                index_minium = i;
            }
        }
        return new int[]{minimum, index_minium};
    }


}

