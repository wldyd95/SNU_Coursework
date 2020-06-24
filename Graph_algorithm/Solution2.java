import java.util.StringTokenizer;
import java.io.FileReader;
import java.io.BufferedReader;
import java.io.PrintWriter;

/*
   1. 아래와 같은 명령어를 입력하면 컴파일이 이루어져야 하며, Solution2 라는 이름의 클래스가 생성되어야 채점이 이루어집니다.
       javac Solution2.java -encoding UTF8

   2. 컴파일 후 아래와 같은 명령어를 입력했을 때 여러분의 프로그램이 정상적으로 출력파일 output2.txt 를 생성시켜야 채점이 이루어집니다.
       java Solution2

   - 제출하시는 소스코드의 인코딩이 UTF8 이어야 함에 유의 바랍니다.
   - 수행시간 측정을 위해 다음과 같이 time 명령어를 사용할 수 있습니다.
       time java Solution2
   - 일정 시간 초과시 프로그램을 강제 종료 시키기 위해 다음과 같이 timeout 명령어를 사용할 수 있습니다.
       timeout 0.5 java Solution2   // 0.5초 수행
       timeout 1 java Solution2     // 1초 수행
 */

class Solution2 {
    static final int MAX_N = 20000;
    static final int MAX_E = 80000;

    static int N, E;
    static int[] U = new int[MAX_E], V = new int[MAX_E], W = new int[MAX_E];
    static int Answer;

    static int[] parent; // used for set manipulation
    static int[] rank; // used for save rank

    public static void main(String[] args) throws Exception {
		/*
		   동일 폴더 내의 input2.txt 로부터 데이터를 읽어옵니다.
		   또한 동일 폴더 내의 output2.txt 로 정답을 출력합니다.
		 */
        BufferedReader br = new BufferedReader(new FileReader("input2.txt"));
        StringTokenizer stk;
        PrintWriter pw = new PrintWriter("output2.txt");

		/*
		   10개의 테스트 케이스가 주어지므로, 각각을 처리합니다.
		 */
        for (int test_case = 1; test_case <= 10; test_case++) {
			/*
			   각 테스트 케이스를 표준 입력에서 읽어옵니다.
			   먼저 정점의 개수와 간선의 개수를 각각 N, E에 읽어들입니다.
			   그리고 각 i번째 간선의 양 끝점의 번호를 U[i], V[i]에 읽어들이고, i번째 간선의 가중치를 W[i]에 읽어들입니다. (0 ≤ i ≤ E-1, 1 ≤ U[i] ≤ N, 1 ≤ V[i] ≤ N)
			 */
            stk = new StringTokenizer(br.readLine());
            N = Integer.parseInt(stk.nextToken()); E = Integer.parseInt(stk.nextToken());
            stk = new StringTokenizer(br.readLine());
            for (int i = 0; i < E; i++) {
                U[i] = Integer.parseInt(stk.nextToken());
                V[i] = Integer.parseInt(stk.nextToken());
                W[i] = Integer.parseInt(stk.nextToken());
            }


            /////////////////////////////////////////////////////////////////////////////////////////////
			/*
			   이 부분에서 여러분의 알고리즘이 수행됩니다.
			   문제의 답을 계산하여 그 값을 Answer에 저장하는 것을 가정하였습니다.
			 */

			// set initialization
            parent = new int[N + 1];
            rank = new int[N + 1];
            parent[0] = 0;
            rank[0] = 0;

            for(int i = 1; i <= N; i++){
                parent[i] = i; // each i vertex makes set i
                rank[i] = 0;
            }

            // for maximum spanning tree, we negate each edge value
            for(int i = 0; i < E; i++){
                W[i] = -W[i];
            }

            // combine U, V, W for convenient sorting
            int[][] UVW = new int[3][E];
			for(int i = 0; i < E; i++){
			    UVW[0][i] = U[i];
                UVW[1][i] = V[i];
                UVW[2][i] = W[i];
            }

            // sort UVW by merge sort
            UVW = mergeSort(UVW, 0, UVW[0].length - 1);

			int count_spanning_tree_edge = 0; // number of edge of spanning tree
            int index = 0;
            int u;
            int v;
            int w;
            int sum = 0; // edge length sum of maximum spanning tree

            ///////////////////////////////////////////////////////////////////////////////////
            // time complexity analysis of Kruskal algorithm
            ///////////////////////////////////////////////////////////////////////////////////

            /*
            In Kruskal algorithm, time complexity is given below
            1. In above Set initialization process is Θ(N), negating edge length is Θ(E), initialization of UVW is Θ(E)
            2. UVW sorting by merge sort is O(E * logE) = O(E * logN)
            3. In below loop, total loop length is O(E) (we may should see all edges)
               In each loop, we used find_set (path compression) and union (rank considered)
               Total number of make_set, union, find_set is O(N + E)
               Therefore, overall loop complexity is O((N + E) * log*N) => almost linear
               In conclusion, total time complexity is O(E * logN)
            */

            // Kruskal algorithm for minimum spanning tree
            while(count_spanning_tree_edge < (N - 1)){
                // pop up minimum edge
                u = UVW[0][index];
                v = UVW[1][index];
                w = UVW[2][index];
                index++;

                if(find_set(u) != find_set(v)){ // u and v are in different set
                    sum += w;
                    count_spanning_tree_edge++;

                    // combine set
                    union(u, v);
                }
            }

            /////////////////////////////////////////////////////////////////////////////////////////////
            Answer = -sum; // maximum spanning tree result

            // output2.txt로 답안을 출력합니다.
            pw.println("#" + test_case + " " + Answer);
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

    // Merge sort
    // edited for UVW in this problem specially
    public static int[][] mergeSort(int[][] A, int p, int r){
        int l = (r - p + 1);
        int[][] sorted;

        if(l == 1) return A; // end of recursion : return only result

        // divide input array into 2 arrays
        int[][] left = new int[3][l / 2];
        int[][] right = new int[3][l - l / 2];

        // allocate each array value
        for(int i = 0; i < l / 2; i++){
            left[0][i] = A[0][i];
            left[1][i] = A[1][i];
            left[2][i] = A[2][i];
        }

        for(int i = 0; i < (l - l / 2); i++){
            right[0][i] = A[0][i + l / 2];
            right[1][i] = A[1][i + l / 2];
            right[2][i] = A[2][i + l / 2];
        }

        left = mergeSort(left, 0, left[0].length - 1);
        right = mergeSort(right, 0, right[0].length - 1);
        sorted = merge(left, right);
        return sorted;
    }

    public static int[][] merge(int[][] left, int[][] right){
        int l = left[0].length;
        int r = right[0].length;
        int[][] merged = new int[3][l + r];

        // allocate index pin of sorted left and right array
        int il = 0; // left array index pin
        int ir = 0; // right array index pin
        int im = 0; // merged array index pin

        while(il < l){
            if(ir < r){
                if(left[2][il] < right[2][ir]) {
                    merged[0][im] = left[0][il];
                    merged[1][im] = left[1][il];
                    merged[2][im] = left[2][il];
                    im++;
                    il++;
                }
                else {
                    merged[0][im] = right[0][ir];
                    merged[1][im] = right[1][ir];
                    merged[2][im] = right[2][ir];
                    im++;
                    ir++;
                }
            }

            // If right array does not exist and only left array survive
            else {
                merged[0][im] = left[0][il];
                merged[1][im] = left[1][il];
                merged[2][im] = left[2][il];
                im++;
                il++;
            }
        }

        // If left array does not exist and only right array survive
        while(ir < r) {
            merged[0][im] = right[0][ir];
            merged[1][im] = right[1][ir];
            merged[2][im] = right[2][ir];
            im++;
            ir++;
        }
        return merged;
    }

    // find root of set (path compression)
    public static int find_set(int x){
        if(parent[x] != x) parent[x] = find_set(parent[x]);
        return parent[x];
    }

    // union of two sets
    public static void union(int x, int y){
        x = find_set(x);
        y = find_set(y);

        if(x == y) return;
        // union using rank
        else{
            if(rank[x] > rank[y]){
                parent[y] = x;
            }

            else{
                parent[x] = y;
                if(rank[x] == rank[y]) rank[y]++;
            }
        }
    }
}

