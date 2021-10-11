import sys, subprocess, random
from statistics import mean

if len(sys.argv) < 3:
    print(f'Digite: python3 {sys.argv[0]} <numero de linhas> <tamanho do bloco>')
    exit(1)

numero_linhas = int(sys.argv[1])
tamanho_bloco = int(sys.argv[2])
qtd_numeros = numero_linhas * tamanho_bloco

with open('entrada.txt', 'w') as file:
    file.write(str(qtd_numeros) + '\n')
    for i in range(numero_linhas):
        for j in range(tamanho_bloco):
            file.write(str(random.randint(10, 99)) + ' ')
        file.write('\n')

nthreads = [1, 2, 4]
num_execucoes = 10

mediaSeq = 0

for n in nthreads:
    resultados = []
    print(f'{n} threads:')
    for i in range(num_execucoes):
        tempo = float(subprocess.run(f'./a {tamanho_bloco} {n}', shell=True, check=True, capture_output=True, text=True).stdout[:-1])
        resultados.append(tempo)
    
    media = mean(resultados)

    if n == 1:
        mediaSeq = media
        print(f'Tempo médio: {media:.6f}s | Aceleração: -\n')
    
    else:
        print(f'Tempo médio: {media:.6f}s | Aceleração: {mediaSeq/media:.6f}\n')