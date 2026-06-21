# Histórico de Prompts (Uso de IA)

Este arquivo documenta os prompts principais utilizados para a geração/refatoração de código com IA neste projeto, em cumprimento aos requisitos da disciplina.

## Refatoração da Arquitetura

**Prompt:**
> Sua tarefa é refatorar o código-fonte atual para melhorar a organização estrutural, a legibilidade e a documentação.
>
> Para garantir que o resultado seja exatamente o que precisamos, obedeça rigorosamente às seguintes restrições:
>PRESERVAÇÃO DE NOMENCLATURA: Nós já estamos familiarizados com o código atual. NÃO altere o nome das variáveis, funções principais ou classes existentes, a menos que haja um erro crasso de digitação. Precisamos continuar nos guiando pelo código com o modelo mental que já temos dele."


---

## Correção da Física da Pista 2

**Prompt 1 (Aviso do Problema):**
> "Ficou ótimo, mas percebi um novo bug. Eu dei uma tacada com toda força a partir do spawn na pista 2 e a bola saiu da pista e caiu rumo ao infinito."

**Prompt 2 (Sugestão da Barreira Invisível):**
> "O bug continuou ocorrendo, dei uma tacada com força e ela saiu da pista indo rumo ao infinito. O que acha de adicionar uma barreira invisivel acima das paredes da pista, de forma que tenha a mesma logica de colisao da parede fisica, porem invisivel?"


## Isolamento do Loop e Gap-Filler Pista 3
PROMPT: A bola ficou presa em uma parede apos o loop e mesmo dando repetidas tacadas a partir dali a bola nem se mexia. Corrija isso sem quebrar o progresso feito até aqui e sem quebrar a pista 2.

## Adição do Zepelin e Curvas de Bézier
**Prompt:**
> "Para dar mais vida ao cenário, quero que você adicione um modelo 3D de um Zepelin voando pelo céu sobre as pistas. Pode gerar/baixar um modelo .obj básico e aplicar uma textura metálica realista nele, não esqueça de colocar a logo do nosso jogo colada na lateral também! A movimentação dele não pode ser reta, preciso que você implemente uma Curva de Bézier cúbica em malha fechada, para que ele faça uma trajetória suave e contínua dando a volta na fase toda. Ajuste a escala para ele ficar visível no céu, e não esqueça de deixar a implementação bem documentada e referenciar as fontes dos assets que você usar"
