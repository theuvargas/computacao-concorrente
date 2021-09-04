import java.util.Arrays;
import java.util.Random;

class T extends Thread {
    private int[] vetor;
    private int[] resultados;
    private int id;

    public static int nthreads;

    public T(int[] vetor, int[] resultados, int id) {
        this.vetor = vetor;
        this.resultados = resultados;
        this.id = id;
    }

    public void run() {
        for (int i = id; i < vetor.length; i += nthreads) {
            if (vetor[i] % 2 == 0) resultados[id] += 1;
        }
    }
}

public class lab6 {
    public static final int LIMITEPRINT = 20; // para tamanho do vetor menor ou igual, printa o vetor

    public static void preencheVetor(int[] vetor, int tamanho) {
        Random rand = new Random();

        for (int i = 0; i < tamanho; i++) {
            vetor[i] = rand.nextInt(10);
        }
    }

    public static void teste(int[] vetor, int resultadoObtido) {
        int quantidadePares = 0;
        for (int num : vetor) {
            if (num % 2 == 0) quantidadePares += 1;
        }

        if (quantidadePares == resultadoObtido) {
            System.out.println("Teste passou");
        }

        else {
            System.out.println("Erro, resultado obtido está incorreto");
        }
    }

    public static void main(String[] args) {
        if (args.length < 2) {
            System.out.println("Erro: É preciso passar como argumento o tamanho do vetor e o número de threads, nessa ordem");
            System.exit(1);
        }

        int tamanho = Integer.parseInt(args[0]);
        T.nthreads = Integer.parseInt(args[1]);

        int[] vetor = new int[tamanho];
        int[] resultados = new int[T.nthreads]; // armazena os resultados das threads

        preencheVetor(vetor, tamanho);

        Thread[] threads = new Thread[T.nthreads];

        for (int i = 0; i < T.nthreads; i++) {
            threads[i] = new T(vetor, resultados, i);
        }

        for (int i = 0; i < T.nthreads; i++) {
            threads[i].start();
        }

        for (int i = 0; i < T.nthreads; i++) {
            try {
                threads[i].join();
            }
            catch (InterruptedException e) {
                System.out.println("Erro join");
                return;
            }
        }

        int resultado = 0;
        for (int resThread : resultados) {
            resultado += resThread;
        }

        if (tamanho <= LIMITEPRINT) {
            System.out.println("Vetor gerado:");
            System.out.println(Arrays.toString(vetor));
        }

        System.out.printf("\nQuantidade de números pares: %d\n", resultado);

        teste(vetor, resultado);
    }
}
