import subprocess, sys

if len(sys.argv) < 2:
    print(f'Digite: python3 {sys.argv[0]} <numero de zeros>')
    exit(1)

tamanhos = []
for i in range(1, len(sys.argv)):
    tamanhos.append(sys.argv[i])

nthreads = [1, 2, 4, 6]
n_execucoes = 5

resultado = []

for tamanho in tamanhos:
    print(f'10^{tamanho} números e {n_execucoes} execuções:\n')
    mediaSeq = 0
    for n in nthreads:
        print(f'{n} threads:')
        for i in range(n_execucoes):
            saida = subprocess.run(f'./a {10**int(tamanho)} {n}', shell=True, check=True, capture_output=True, text=True)    
            resultado.append(float(saida.stdout))
            
            media = sum(resultado)/len(resultado)
            minimo = min(resultado)

        if n == 1:
            mediaSeq = media
            print(f'Tempo mínimo: {minimo:.6f}s | Tempo médio: {media:.6f}s | Aceleração: -\n')

        else:
            print(f'Tempo mínimo: {minimo:.6f}s | Tempo médio: {media:.6f}s | Aceleração: {mediaSeq/media:.6f}\n')

        resultado = []
