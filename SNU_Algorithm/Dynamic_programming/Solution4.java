import java.util.StringTokenizer;
import java.io.FileReader;
import java.io.BufferedReader;
import java.io.PrintWriter;

/*
   1. 아래와 같은 명령어를 입력하면 컴파일이 이루어져야 하며, Solution4 라는 이름의 클래스가 생성되어야 채점이 이루어집니다.
       javac Solution4.java -encoding UTF8


   2. 컴파일 후 아래와 같은 명령어를 입력했을 때 여러분의 프로그램이 정상적으로 출력파일 output4.txt 를 생성시켜야 채점이 이루어집니다.
       java Solution4

   - 제출하시는 소스코드의 인코딩이 UTF8 이어야 함에 유의 바랍니다.
   - 수행시간 측정을 위해 다음과 같이 time 명령어를 사용할 수 있습니다.
       time java Solution4
   - 일정 시간 초과시 프로그램을 강제 종료 시키기 위해 다음과 같이 timeout 명령어를 사용할 수 있습니다.
       timeout 0.5 java Solution4   // 0.5초 수행
       timeout 1 java Solution4     // 1초 수행
 */

class Solution4 {
    static final int max_n = 100000;

    static int n;
    static int[][] A = new int[3][max_n];
    static int Answer;

    public static void main(String[] args) throws Exception {
		/*
		   동일 폴더 내의 input4.txt 로부터 데이터를 읽어옵니다.
		   또한 동일 폴더 내의 output4.txt 로 정답을 출력합니다.
		 */
        BufferedReader br = new BufferedReader(new FileReader("input4.txt"));
        StringTokenizer stk;
        PrintWriter pw = new PrintWriter("output4.txt");

		/*
		   10개의 테스트 케이스가 주어지므로, 각각을 처리합니다.
		 */
        for (int test_case = 1; test_case <= 10; test_case++) {
			/*
			   각 테스트 케이스를 표준 입력에서 읽어옵니다.
			   먼저 놀이판의 열의 개수를 n에 읽어들입니다.
			   그리고 첫 번째 행에 쓰여진 n개의 숫자를 차례로 A[0][0], A[0][1], ... , A[0][n-1]에 읽어들입니다.
			   마찬가지로 두 번째 행에 쓰여진 n개의 숫자를 차례로 A[1][0], A[1][1], ... , A[1][n-1]에 읽어들이고,
			   세 번째 행에 쓰여진 n개의 숫자를 차례로 A[2][0], A[2][1], ... , A[2][n-1]에 읽어들입니다.
			 */
            stk = new StringTokenizer(br.readLine());
            n = Integer.parseInt(stk.nextToken());
            for (int i = 0; i < 3; i++) {
                stk = new StringTokenizer(br.readLine());
                for (int j = 0; j < n; j++) {
                    A[i][j] = Integer.parseInt(stk.nextToken());
                }
            }

            /////////////////////////////////////////////////////////////////////////////////////////////
			/*
			   이 부분에서 여러분의 알고리즘이 수행됩니다.
			   문제의 답을 계산하여 그 값을 Answer 에 저장하는 것을 가정하였습니다.
			 */

			// for solving play board problem, we will use dynamic programming
            // there are three possible symbols : P(positive, plus), Z(zero), N(negative, minus)
            // we can make 6 configuration s using these symbols
            // 6 configuration : PZN, PNZ, ZPN, ZNP, NPZ, NZP
            // in below table for example, PZN[i] : maximum point when ith column is in PZN configuration

            // for each configuration, possible left configuration is like below
            // PZN : ZNP, NPZ
            // PNZ : ZPN, NZP
            // ZPN : PNZ, NZP
            // ZNP : PZN, NPZ
            // NPZ : PZN, ZNP
            // NZP : PNZ, ZPN

            ///////////////////////////////////////////////////////////////

            // time complexity analysis of algorithm
            // initialization needs just constant time O(1)
            // in main algorithm, for calculating each value there are find_maximum of 2 different values
            // therefore in main algorithm for calculating 6 table elements 6 * n time is needed..O(n)
            // from above discussion total time complexity is O(n) (linear)

            ///////////////////////////////////////////////////////////////

            int[] PZN = new int[n];
            int[] PNZ = new int[n];
            int[] ZPN = new int[n];
            int[] ZNP = new int[n];
            int[] NPZ = new int[n];
            int[] NZP = new int[n];

            // initialization
            PZN[0] = A[0][0] * ( 1 ) + A[1][0] * ( 0 ) + A[2][0] * ( -1 );
            PNZ[0] = A[0][0] * ( 1 ) + A[1][0] * ( -1 ) + A[2][0] * ( 0 );
            ZPN[0] = A[0][0] * ( 0 ) + A[1][0] * ( 1 ) + A[2][0] * ( -1 );
            ZNP[0] = A[0][0] * ( 0 ) + A[1][0] * ( -1 ) + A[2][0] * ( 1 );
            NPZ[0] = A[0][0] * ( -1 ) + A[1][0] * ( 1 ) + A[2][0] * ( 0 );
            NZP[0] = A[0][0] * ( -1 ) + A[1][0] * ( 0 ) + A[2][0] * ( 1 );

            // main algorithm
            // move by column
            for(int j = 1; j < n; j++){
                PZN[j] = find_maximum( new int[] { ZNP[j - 1], NPZ[j - 1] } ) + ( A[0][j] * ( 1 ) + A[1][j] * ( 0 ) + A[2][j] * ( -1 ) );
                PNZ[j] = find_maximum( new int[] { ZPN[j - 1], NZP[j - 1] } ) + ( A[0][j] * ( 1 ) + A[1][j] * ( -1 ) + A[2][j] * ( 0 ) );
                ZPN[j] = find_maximum( new int[] { PNZ[j - 1], NZP[j - 1] } ) + ( A[0][j] * ( 0 ) + A[1][j] * ( 1 ) + A[2][j] * ( -1 ) );
                ZNP[j] = find_maximum( new int[] { PZN[j - 1], NPZ[j - 1] } ) + ( A[0][j] * ( 0 ) + A[1][j] * ( -1 ) + A[2][j] * ( 1 ) );
                NPZ[j] = find_maximum( new int[] { PZN[j - 1], ZNP[j - 1] } ) + ( A[0][j] * ( -1 ) + A[1][j] * ( 1 ) + A[2][j] * ( 0 ) );
                NZP[j] = find_maximum( new int[] { PNZ[j - 1], ZPN[j - 1] } ) + ( A[0][j] * ( -1 ) + A[1][j] * ( 0 ) + A[2][j] * ( 1 ) );
            }

            /////////////////////////////////////////////////////////////////////////////////////////////
            Answer = find_maximum(new int[]{ PZN[n - 1], PNZ[n - 1], ZPN[n - 1], ZNP[n - 1], NPZ[n - 1], NZP[n - 1] });


            // output4.txt로 답안을 출력합니다.
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

    public static int find_maximum(int[] A){
        int maximum = A[0];

        for(int i = 0; i < A.length; i++){
            if(A[i] >= maximum){
                maximum = A[i];
            }
        }
        return maximum;
    }

}

