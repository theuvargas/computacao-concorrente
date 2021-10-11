import subprocess, random, sys

if len(sys.argv) < 2:
    print(f'Digite: python3 {sys.argv[0]} <numero de threads>')
    exit(1)

nthreads = sys.argv[1]

ordenado = []
for i in range(1, 101):
    ordenado.append(i)

decrescente = []
for i in range(100, -1, -1):
    decrescente.append(i)

aleatorio = []
for i in range(1, 101):
    aleatorio.append(random.randint(-1000, 1000))

numero_linhas = 3
tamanho_bloco = 100
qtd_numeros = numero_linhas * tamanho_bloco

with open('entrada.txt', 'w') as file:
    file.write(str(qtd_numeros) + '\n')
    for num in ordenado:
        file.write(str(num) + ' ')
    file.write('\n')

    for num in decrescente:
        file.write(str(num) + ' ')
    file.write('\n')

    for num in aleatorio:
        file.write(str(num) + ' ')
    file.write('\n')

subprocess.run(f'./a {tamanho_bloco} {nthreads}', shell=True, check=True, capture_output=True)

linhas = []
with open('saida.txt', 'r') as file:
    for i in range(numero_linhas):
        linha = file.readline().split(' ')
        linha = list(map(int, linha))
        linhas.append(linha)

for linha in linhas:
    if not linha == sorted(linha):
        print('Ordenação incorreta em uma das listas')
        exit()

print('As listas foram ordenadas corretamente')