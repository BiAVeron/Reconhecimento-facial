# Instruções para Execução do Projeto - Reconhecimento Facial com Árvore KD

## Visão Geral

Este projeto implementa um sistema de reconhecimento facial utilizando uma estrutura de dados KD-Tree desenvolvida em C e integrada com uma API Python construída com FastAPI. O sistema recebe imagens faciais, extrai embeddings com o DeepFace e permite buscar a pessoa mais próxima com base nesses vetores.

---

## Requisitos

* Python 3.8+
* GCC (para compilar a biblioteca em C)
* VS Code (ou editor de sua preferência)
* Extensão Jupyter (caso utilize o `.ipynb`)

### Instalação de bibliotecas Python

```bash
pip install -r requirements.txt
```

Ou instale manualmente:

```bash
pip install fastapi uvicorn requests deepface
```

---

## Estrutura dos Arquivos

```
meu-projeto/
├── api/
│   ├── app.py                # API principal com FastAPI
│   └── kdtree_wrapper.py     # Wrapper com ctypes para acessar o código 
│   └── kdtree.c              # Implementação da árvore KD em C 
|   └── libkdtree.dll         # Biblioteca compilada (ou gerar via gcc)   
├── data/
│   ├── beatriz.jpg           # Imagem pessoal para teste de reconhecimento
│   ├── gabi.jpg
|   ├── rose.jpg              # Outra imagem de teste
│   └── lfw-funneled.tgz      # Base de dados com rostos conhecidos
│           
├── Reconhecimento_Facial_Kd.ipynb      # Notebook usado para testes no Colab           
├── requirements.txt          # Bibliotecas Python necessárias
└── README.md                 # Este arquivo
```

---

## Organização dos dados

As imagens usadas nos testes estão na pasta `data/`. Lá estão:

* `beatriz.jpg`, `rose.jpg` e `gabi.jpg` – usados nos testes manuais;
* A subpasta `lfw_faces/lfw_funneled/` – com os rostos da base LFW. Essa deve ser baixada e colacada dentro da pasta `data/`

---

## Compilando o código C

Em sistemas Linux ou WSL:

```bash
gcc -shared -o libkdtree.dll -fPIC c_kdtree/kdtree.c
```

---

## Rodando a API

Execute o seguinte comando:

```bash
uvicorn api.app:app --reload
```

A API ficará disponível em: `http://127.0.0.1:8000`

---

## Testando com o notebook Colab

No Google Colab:

1. Aponte a URL do `ngrok` para seu `localhost:8000`
2. Execute o notebook `deepface_colab.ipynb`
3. Ele realiza inserções na árvore KD e busca facial via API.

---

## Observações

* O vídeo de apresentação está na pasta do Google Drive.
* A pasta também contém o notebook `.ipynb` usado para os testes.
* A parte opcional da implementação de heap não foi realizada.

---

## Contato

Beatriz – Projeto apresentado para disciplina de Estrutura de Dados.
