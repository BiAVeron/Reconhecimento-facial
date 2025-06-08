# app.py
from fastapi import FastAPI
from ctypes import c_float, byref
from kdtree_wrapper import lib, Tarv, TReg
from pydantic import BaseModel

app = FastAPI()

class PontoEntrada(BaseModel):
    embedding: list[float]
    id: str

@app.get("/")
async def root():
    return {"message": "API está rodando!"}

@app.post("/construir-arvore")
def constroi_arvore():
    lib.kdtree_construir()
    return {"mensagem": "Árvore KD inicializada com sucesso."}

@app.post("/inserir")
def inserir(ponto: PontoEntrada):
    try:
        print(f"Recebido ID: {ponto.id}")
        print(f"Tamanho do embedding: {len(ponto.embedding)}")

        if len(ponto.embedding) != 128:
            return {"erro": "O vetor de embedding deve conter exatamente 128 valores."}

        emb_array = (c_float * 128)(*ponto.embedding)
        id_bytes = ponto.id.encode('utf-8')[:99] + b'\0'
        novo_ponto = TReg(embedding=emb_array, id=id_bytes)

        lib.inserir_ponto(novo_ponto)

        return {"mensagem": f"Pessoa com ID '{ponto.id}' inserida com sucesso."}
    except Exception as e:
        print(f"Erro ao inserir ponto: {e}")
        return {"erro": "Erro interno no servidor", "detalhe": str(e)}

@app.post("/buscar")
def buscar(entrada: PontoEntrada):
    try:
        embedding = entrada.embedding

        if len(embedding) != 128:
            return {"erro": "O vetor de embedding deve conter exatamente 128 valores."}

        emb_array = (c_float * 128)(*embedding)
        consulta = TReg(embedding=emb_array, id=b'\0')
        arv = lib.get_tree()

        if not arv or not arv.contents or not hasattr(arv.contents, "raiz") or not arv.contents.raiz:
            return {"erro": "Árvore KD não inicializada ou está vazia."}

        resultado = lib.buscar_mais_proximo(arv, byref(consulta))

        # 2. Verifique se o ponteiro não é nulo
        if not resultado:
            return {"erro": "Nenhum ponto encontrado na árvore."}

        print(f"ID mais próximo encontrado: {resultado.id.decode('utf-8').rstrip(chr(0))}")

        return {"id": resultado.id.decode('utf-8').rstrip('\x00')}

    except Exception as e:
        import traceback
        return {
            "erro": "Erro interno no servidor.",
            "detalhe": str(e),
            "traceback": traceback.format_exc()
        }
