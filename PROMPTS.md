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

## Refinamento do Multiplayer e Efeitos Visuais
**Prompt:**
> "Ok, gostaria de uma modificação no multiplayer agora. Primeiramente, ele me parece estar bem quebrado, de forma que nao altera corretamente os turnos. A pista acaba quando ambos os players colocarem a bola no buraco, caso um deles ja tenha conseguido, para de alternar os turnos e só o que ainda nao conseguiu joga, ao final vamos para uma tela indicando qual player venceu, o numero de tacadas de cada player e exibindo as opcoes de proxima pista ou retornar ao menu (para o single player, o texto está ótimo, mas gostaria de adicionar o numero de tacadas para o jogador ter uma noção de como foi seu desempenho). Sobre a colisão entre as bolinhas, ela nao deve ocorrer, quando a vez for de um player, a bolinha do outro deverá estar translucida e com um indicador visual em sua superficie indicando que ela de fato esta inatingivel (como aneis circulares passando pela esfera, indicando a sua intangibilidade)."
