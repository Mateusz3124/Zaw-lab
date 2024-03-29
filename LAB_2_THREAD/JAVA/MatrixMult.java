import java.io.File;
import java.io.FileNotFoundException;
import java.util.Locale;
import java.util.Scanner;

import static java.lang.Math.sqrt;

public class MatrixMult {

    public float sumOfElementsOfMatrixResult = 0;
    private static final Object lock = new Object();
    public double sumOfSquares;

    public static void main(String[] args) {
        MatrixMult mm = new MatrixMult();

        try {
            mm.start(args);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    protected void start(String[] args) throws Exception {
        if(args == null){
            throw new IllegalArgumentException("Argument jest nullem");
        }
        Matrix A, B;
        if (args.length != 3) {
            throw new IllegalArgumentException("Wprowadzono za malo argumentow: <nazwa pliku> <nazwa pliku> <liczba watkow>");
        }
        int numberOfThreads;
        try{
        A = read(args[0]);
        B = read(args[1]);
        } catch (Exception e) {
            throw new Exception("Blad podczas odczytu danych: "+ e);
        }
        if(A.cols() != B.rows()){
            throw new IllegalArgumentException("rozmiary macierzy sa nieprawidlowe, mnozenie jest niezmozliwe");
        }
        if(A.rows() > 1000 || B.rows() > 1000 || A.cols() > 1000 || B.cols() > 1000){
            throw new Exception("Za duzy rozmiar macierzy");
        }
        if(A.rows() <= 0 || B.rows() <= 0 || A.cols() <= 0 || B.cols() <= 0){
            throw new Exception("Rozmiar macierzy jest rowny 0 lub mniejszy od 0");
        }
        try {
            numberOfThreads = Integer.parseInt(args[2]);
        } catch (NumberFormatException e) {
            throw new NumberFormatException("Ostatni argument nie jest liczbą");
        }
        if (numberOfThreads <= 0 || numberOfThreads > A.nrows) {
            throw new IllegalArgumentException("Liczba watkow jest nieprawidlowa");
        }

        System.out.println("Wczytalem A:");
        print(A);

        System.out.println("\nWczytalem B:");
        print(B);

        Matrix C = mult(A, B, numberOfThreads);
        System.out.println("A*B = ");

        print(C);

        System.out.printf("Suma elementow wyniku: %.1f\n", sumOfElementsOfMatrixResult);
        System.out.printf("Frobenius norm: %.1f\n", sqrt(sumOfSquares));
    }

    private Matrix mult(Matrix A, Matrix B, int numThreads) throws Exception{
        Matrix C = new Matrix(A.rows(), B.cols());

        int[] worksPerGroup = new int[numThreads];
        int work_size = A.nrows / numThreads;

        for (int i = 0; i < numThreads; i++) {
            worksPerGroup[i] = work_size;
        }

        int rest = A.nrows - numThreads * work_size;
        for (int i = 0; i < rest; i++) {
            worksPerGroup[i % numThreads]++;
        }

        Thread[] threads = new Thread[numThreads];
        MultThread[] multThread = new MultThread[numThreads];
        try {
            for (int i = 0; i < numThreads; i++) {
                multThread[i] = new MultThread(A, B, C, i, worksPerGroup);
                threads[i] = new Thread(multThread[i]);
                threads[i].start();
            }
        } catch (Exception e) {
            throw new Exception("Blad podczas rozpoczynania watkow: " + e);
        }
        sumOfSquares = 0.0;
        for (int i = 0; i < numThreads; i++) {
            try {
                threads[i].join();
                sumOfSquares += multThread[i].sumOfSquares;
            } catch (InterruptedException e) {
                throw e;
            }
        }

        return C;
    }

    protected Matrix read(String fname) throws FileNotFoundException {
        File f = new File(fname);
        Scanner scanner = new Scanner(f).useLocale(Locale.ENGLISH);

        int rows = scanner.nextInt();
        int cols = scanner.nextInt();
        Matrix res = new Matrix(rows, cols);

        for (int r = 0; r < res.rows(); r++) {
            for (int c = 0; c < res.cols(); c++) {
                res.set(r, c, scanner.nextFloat());
            }
        }
        return res;
    }

    protected void print(Matrix m) {
        System.out.println("[");
        for (int r = 0; r < m.rows(); r++) {

            for (int c = 0; c < m.cols(); c++) {
                System.out.print(m.get(r, c));
                System.out.print(" ");
            }

            System.out.println("");
        }
        System.out.println("]");
    }

    private class MultThread implements Runnable {
        Matrix A;
        Matrix B;
        Matrix C;
        int iteration;
        int[] worksPerGroup;
        double sumOfSquares;

        public MultThread(Matrix A, Matrix B, Matrix C, int iteration, int[] worksPerGroup) {
            this.A = A;
            this.B = B;
            this.C = C;
            this.iteration = iteration;
            this.worksPerGroup = worksPerGroup;
        }

        @Override
        public void run() {
            int startRowA = 0;
            for (int i = 0; i < iteration; i++) {
                startRowA += worksPerGroup[i];
            }
            sumOfSquares = 0.0;
            float localSumOfElementsOfMatrixResult = 0;
            for (int i = startRowA; i < startRowA + worksPerGroup[iteration]; i++) {
                for (int column = 0; column < B.ncols; column++) {
                    float sum = 0;
                    for (int k = 0; k < A.ncols; k++) {
                        sum += A.get(i, k) * B.get(k, column);
                    }
                    C.set(i, column, sum);
                    localSumOfElementsOfMatrixResult += sum;
                    sumOfSquares += sum * sum;
                }
            }
            synchronized(lock){
                sumOfElementsOfMatrixResult += localSumOfElementsOfMatrixResult;
            }
            return;
        }
    }

    public class Matrix {
        private int ncols;
        private int nrows;
        private float _data[];

        public Matrix(int r, int c) {
            this.ncols = c;
            this.nrows = r;
            _data = new float[c * r];
        }

        public float get(int r, int c) {
            return _data[r * ncols + c];
        }

        public void set(int r, int c, float v) {
            _data[r * ncols + c] = v;
        }

        public int rows() {
            return nrows;
        }

        public int cols() {
            return ncols;
        }
    }


} 
