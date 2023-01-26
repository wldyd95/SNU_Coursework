import java.util.StringTokenizer;
import java.io.FileReader;
import java.io.BufferedReader;
import java.io.PrintWriter;

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

    static final int MAX_N = 1000;
    static final int MAX_E = 100000;
    static final int Div = 100000000;  // 1억
    static int N, E;
    static int[] U = new int[MAX_E], V = new int[MAX_E], W = new int[MAX_E];
    static int[] Answer1 = new int[MAX_N+1];
    static int[] Answer2 = new int[MAX_N+1];
    static double start1, start2;
    static double time1, time2;


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
			   먼저 정점의 개수와 간선의 개수를 각각 N, E에 읽어들입니다.
			   그리고 각 i번째 간선의 시작점의 번호를 U[i], 끝점의 번호를 V[i]에, 간선의 가중치를 W[i]에 읽어들입니다.
			   (0 ≤ i ≤ E-1, 1 ≤ U[i] ≤ N, 1 ≤ V[i] ≤ N)
			 */
            stk = new StringTokenizer(br.readLine());
            N = Integer.parseInt(stk.nextToken()); E = Integer.parseInt(stk.nextToken());
            stk = new StringTokenizer(br.readLine());
            for (int i = 0; i < E; i++) {
                U[i] = Integer.parseInt(stk.nextToken());
                V[i] = Integer.parseInt(stk.nextToken());
                W[i] = Integer.parseInt(stk.nextToken());
            }

            /* Problem 1-1 */
            start1 = System.currentTimeMillis();
            Answer1 = BellmanFord1(U, V, W);
            time1 = (System.currentTimeMillis() - start1);

            /* Problem 1-2 */
            start2 = System.currentTimeMillis();
            Answer2 = BellmanFord2(U, V, W);
            time2 = (System.currentTimeMillis() - start2);

            // output1.txt로 답안을 출력합니다.
            pw.println("#" + test_case);
            for (int i = 1; i <= N; i++) {
                pw.print(Answer1[i]);
                if (i != N)
                    pw.print(" ");
                else
                    pw.print("\n");
            }
            pw.println(time1);

            for (int i = 1; i <= N; i++) {
                pw.print(Answer2[i]);
                if (i != N)
                    pw.print(" ");
                else
                    pw.print("\n");
            }
            pw.println(time2);
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

    //////////////////////////////////////////////////////////////////////////////////////////////
    // time complexity analysis of BellmanFord1 algorithm
    //////////////////////////////////////////////////////////////////////////////////////////////

    /*
    In below BellmanFord1 (original BellmanFord) algorithm, for initialization θ(N) time is needed
    For each relaxation process, we should see all E edges, and this edge looking loop iterates (N - 1) times
    Therefore, time complexity of BellmanFord1 algorithm is θ(E * N)
    */


    public static int[] BellmanFord1(int[] U, int[] V, int[] W){
        int[] d = new int[N + 1]; // this array saves shortest distance value of each vertex
        for(int i = 0; i < (N + 1); i++){
            d[i] = 10000000; // initialization, every shortest path cannot be larger than this value
        }

        d[0] = 0; // just zero value, no meaning
        d[1] = 0; // we start from vertex 1

        for(int i = 1; i <= (N - 1); i++){

            for(int j = 0; j < E; j++){

                if( d[U[j]] + W[j] < d[V[j]] ) d[V[j]] = d[U[j]] + W[j]; // relaxation

            }

        }


        for(int i = 0; i < (N + 1); i++){
            d[i] = d[i] % Div;
        }

        return d;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    // time complexity analysis of BellmanFord2 algorithm
    //////////////////////////////////////////////////////////////////////////////////////////////

    /*
    In below BellmanFord2 algorithm, for initialization θ(N) time is needed
    For initialization of adjacency, edge_length, edge_number θ(E) time is needed
    For each relaxation process, seeing only relaxed vertex is enough, therefore edge looking process takes less than E calculation
    This edge looking process iterates (N - 1) times
    Therefore total time complexity is O(E * N) => better than BellmanFord1!
    */


    public static int[] BellmanFord2(int[] U, int[] V, int[] W){
        int[] d = new int[N + 1]; // this array saves shortest distance value of each vertex
        for(int i = 0; i < (N + 1); i++) {
            d[i] = 10000000; // initialization, every shortest path cannot be larger than this value
        }
        d[0] = 0; // just zero value, no meaning
        d[1] = 0; // we start from vertex 1

        // first element of each array is 0..because we start from vertex 1
        int[][] adjacency = new int[N + 1][N + 1]; // saves adjacency information, if not adjacent 0
        int[][] edge_length = new int[N + 1][N + 1]; // saves edge length information
        int[] edge_number = new int[N + 1]; // saves connected edge number of each vertex

        // read information from U, V, W
        for(int i = 0; i < E; i++){
            adjacency[U[i]][edge_number[U[i]] + 1] = V[i];
            edge_length[U[i]][edge_number[U[i]] + 1] = W[i];
            edge_number[U[i]]++;
        }

        // now we need queue for efficient loop
        int[] queue = new int[N * N]; // large enough array
        int front = 0;
        int rear = 0;

        // queue initialization, we start from vertex 1
        queue[rear++] = 1;
        int r;

        // main loop
        for(int i = 1; i <= (N - 1); i++){
            int count = 0; // relaxation count

            while(front != rear){ // do loop for each element of queue
                r = queue[front++]; // read vertex which was relaxed in last loop

                for(int j = 1; j <= edge_number[r]; j++){ // see all edge connected to r
                    if(d[r] + edge_length[r][j] < d[adjacency[r][j]]){
                        d[adjacency[r][j]] = d[r] + edge_length[r][j]; // relaxation
                        queue[rear + count] = adjacency[r][j]; // if relaxation occur, add it to queue
                        count++;
                    }
                }
            }

            // update rear
            rear += count;
        }

        for(int i = 0; i < (N + 1); i++){
            d[i] = d[i] % Div;
        }

        return d;
    }

}
