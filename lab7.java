// Monitor que implementa a logica do padrao leitores/escritores
class LE {
    private int leit, escr;

    // Construtor
    LE() {
        this.leit = 0; // leitores lendo (0 ou mais)
        this.escr = 0; // escritor escrevendo (0 ou 1)
    }

    // Entrada para leitores
    public synchronized void EntraLeitor (int id) {
        try {
            while (this.escr > 0) {
                wait();  //bloqueia pela condicao logica da aplicacao
            }
            this.leit++;  //registra que ha mais um leitor lendo
        } catch (InterruptedException e) { }
    }

    // Saida para leitores
    public synchronized void SaiLeitor (int id) {
        this.leit--; //registra que um leitor saiu
        if (this.leit == 0)
            this.notify(); //libera escritor (caso exista escritor bloqueado)
    }

    // Entrada para escritores
    public synchronized void EntraEscritor (int id) {
        try {
            while ((this.leit > 0) || (this.escr > 0)) {
                wait();  //bloqueia pela condicao logica da aplicacao
            }
            this.escr++; //registra que ha um escritor escrevendo
        } catch (InterruptedException e) { }
    }

    // Saida para escritores
    public synchronized void SaiEscritor (int id) {
        this.escr--; //registra que o escritor saiu
        notifyAll(); //libera leitores e escritores (caso existam leitores ou escritores bloqueados)
    }
}

// Leitor
class Leitor extends Thread {
    final int id; // identificador da thread
    LE monitor;// objeto monitor para coordenar a lógica de execução das threads

    static boolean ehPrimo(int n) {
        if (n <= 1) return false;
        for (int i = 2; i <= Math.sqrt(n); i++) {
            if ((float) n / i % 1 == 0) return false;
        }
        return true;
    }

    // Construtor
    Leitor (int id, LE monitor) {
        this.id = id;
        this.monitor = monitor;
    }

    // Método executado pela thread
    public void run () {
        while (true){
            this.monitor.EntraLeitor(this.id);

            if (ehPrimo(lab7.numero)) {
                System.out.printf("Leitor %d: %d - é primo\n", this.id, lab7.numero);
            } else {
                System.out.printf("Leitor %d: %d - não é primo\n", this.id, lab7.numero);
            }
            this.monitor.SaiLeitor(this.id);
            try {
                sleep(1000);
            } catch (InterruptedException ignored) {
            }
        }
    }
}

class Escritor extends Thread {
    final int id; // identificador da thread
    LE monitor; // objeto monitor para coordenar a lógica de execução das threads

    // Construtor
    Escritor (int id, LE monitor) {
        this.id = id;
        this.monitor = monitor;
    }

    // Método executado pela thread
    public void run () {
        while (true) {
            this.monitor.EntraEscritor(this.id);
            lab7.numero = this.id;
            System.out.printf("Escritor %d escreveu %d\n", this.id, this.id);
            this.monitor.SaiEscritor(this.id);
            try {
                sleep(1000);
            } catch (InterruptedException ignored) {
            }
        }
    }
}

// thread mista
class LeitorEscritor extends Thread {
    final int id;
    LE monitor;

    public LeitorEscritor(int id, LE monitor) {
        this.id = id;
        this.monitor = monitor;
    }

    public void run () {
        while (true) {
            this.monitor.EntraLeitor(this.id);

            if (lab7.numero % 2 == 0) {
                System.out.printf("Misto %d: %d - é par\n", this.id, lab7.numero);
            } else {
                System.out.printf("Misto %d: %d - é ímpar\n", this.id, lab7.numero);
            }
            this.monitor.SaiLeitor(this.id);

            this.monitor.EntraEscritor(this.id);
            lab7.numero *= 2;
            System.out.printf("Misto %d escreveu %d\n", this.id, lab7.numero);
            this.monitor.SaiEscritor(this.id);

            try {
                sleep(1000);
            } catch (InterruptedException ignored) {
            }
        }
    }
}

public class lab7 {
    static final int L = 4;
    static final int E = 2;
    static final int LeE = 2;

    static int numero = 0;

    public static void main (String[] args) {
        int i;
        LE monitor = new LE();                          // Monitor (objeto compartilhado entre leitores e escritores)
        Leitor[] l = new Leitor[L];                     // Threads leitores
        Escritor[] e = new Escritor[E];                 // Threads escritores
        LeitorEscritor[] le = new LeitorEscritor[LeE];  // Threads mistas

        for (i=0; i<L; i++) {
            l[i] = new Leitor(i+1, monitor);
            l[i].start();
        }
        for (i=0; i<E; i++) {
            e[i] = new Escritor(i+1, monitor);
            e[i].start();
        }
        for (i=0; i<LeE; i++) {
            le[i] = new LeitorEscritor(i+1, monitor);
            le[i].start();
        }
    }
}