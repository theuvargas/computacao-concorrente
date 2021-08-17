import subprocess, sys

nthreads = sys.argv[1]

tamanhos = [6, 7, 8]
n_execucoes = 5

resultado = []

print(f'{nthreads} threads e {n_execucoes} execuções:\n')

for n in tamanhos:
    print(f'Tamanho: 10^{n}')
    for i in range(n_execucoes):
        s = subprocess.check_output(f'./mergesort {10**n} {nthreads}', shell = True)    
        resultado.append(float(s.decode('ascii')))
        
        mediaConc = sum(resultado)/len(resultado)
        minConc = min(resultado)

    print(f'Tempo médio: {round(mediaConc, 6)}s | Tempo mínimo: {round(minConc, 6)}s\n')
    resultado = []
