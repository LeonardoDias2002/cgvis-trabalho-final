# Histórico de Prompts (Uso de IA)

Este arquivo documenta os prompts principais utilizados para a geração/refatoração de código com IA neste projeto, em cumprimento aos requisitos da disciplina.

## Refatoração da Arquitetura

**Prompt:**
> "Você é um engenheiro de software sênior com especialização em Computação Gráfica e excelente didática acadêmica.
>
> Você tem acesso à pasta do meu projeto de computação gráfica. Este é um trabalho de faculdade desenvolvido por mim e meu colega, portanto, o objetivo principal não é criar um produto de software comercial com arquitetura complexa, mas sim um projeto funcional, organizado e fácil de entender.
>
> Sua tarefa é refatorar o código-fonte atual para melhorar a organização estrutural, a legibilidade e a documentação.
>
> Para garantir que o resultado seja exatamente o que precisamos, obedeça rigorosamente às seguintes restrições:
>
> <constraints>
> 1. PRESERVAÇÃO DE NOMENCLATURA (CRÍTICO): Nós já estamos familiarizados com o código atual. NÃO altere o nome das variáveis, funções principais ou classes existentes, a menos que haja um erro crasso de digitação. Precisamos continuar nos guiando pelo código com o modelo mental que já temos dele."

**Prompt Secundário:**
> "Refatore o código desse projeto para deixa-lo mais limpo e legível (atualmente, temos um main.cpp gigante, imagino que seja mais inteligente separar para deixar algo mais bonito - não precisa fazer algo extremamente orientado a objetos necessariamente, mas deixar o código mais organizado e bem documentado)"

---

## Correção da Física da Pista 2

**Prompt 1 (Aviso do Problema):**
> "Ficou ótimo, mas percebi um novo bug. Eu dei uma tacada com toda força a partir do spawn na pista 2 e a bola saiu da pista e caiu rumo ao infinito."

**Prompt 2 (Sugestão da Barreira Invisível):**
> "O bug continuou ocorrendo, dei uma tacada com força e ela saiu da pista indo rumo ao infinito. O que acha de adicionar uma barreira invisivel acima das paredes da pista, de forma que tenha a mesma logica de colisao da parede fisica, porem invisivel?"

**Prompt 3 (Reforço das Restrições de Limite):**
> "O bug continua. Dependendo da força do taco ela sobe, sai da pista e cai rumo ao infinito. Ela não pode, sob hipotese alguma, jamais, sair dos limites da pista, que é definido (no caso da pista 2) pelas paredes da mesma."

## Isolamento do Loop e Gap-Filler Pista 3
PROMPT: Nao foi para debaixo da rampa, mas ficou presa em uma parede apos o loop e mesmo dando repetidas tacadas a partir dali a bola nem se mexia. Corrija isso sem quebrar o progresso feito até aqui e sem quebrar a pista 2. ALém disso, notei um detalhe na especificação do professor: Lembre-se que os testes de colisão devem ser implementados em um arquivo à parte, nomeado collisions.cpp.
