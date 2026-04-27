# Especificação da Implementação

> [!CAUTION]
> - Você <ins>**não pode utilizar ferramentas de IA para escrever esta
>   especificação**</ins>

## Integrantes da dupla

- **Aluno 1 - Nome**: <mark>`Leonardo Leal Linhares Dias`</mark>
- **Aluno 1 - Cartão UFRGS**: <mark>`326011`</mark>

- **Aluno 2 - Nome**: <mark>`Gabriel Iza Fauth`</mark>
- **Aluno 2 - Cartão UFRGS**: <mark>`579340`</mark>

## Detalhes do que será implementado

- **Título do trabalho**: <mark>`GOLFinho`</mark>
- **Parágrafo curto descrevendo o que será implementado**: <mark>`Um jogo de minigolf de 2 jogadores, em que os jogadores se revezam em turnos para dar as suas tacadas em um circuito de 5 pistas. `</mark>

## Especificação visual

### Vídeo - Link

> [!IMPORTANT]
> - Coloque aqui um link para um vídeo que mostre a aplicação gráfica
>   de referência que você vai implementar. **Sua implementação deverá
>   ser o mais parecido possível com o que é mostrado no vídeo (mais
>   detalhes abaixo).**
> - **Você não pode escolher como referência: (1) algum trabalho realizado
>   por outros alunos desta disciplina, em semestres anteriores. (2) Minecraft.**
> - Por exemplo, você pode colocar um vídeo de um jogo que você gosta,
>   e seu trabalho final será uma re-implementação do jogo.
> - O vídeo pode ser um link para YouTube, Google Drive, ou arquivo mp4 dentro
>   do próprio repositório. Mas, garanta que qualquer um tenha
>   permissão de acesso ao vídeo através deste link.

<mark>`https://www.youtube.com/watch?v=eNc0Ef0jBZc&t=30s`</mark>

### Vídeo - Timestamp

> [!IMPORTANT]
> - Coloque aqui um **intervalo de ~30 segundos** do vídeo acima, que
>   será a base de comparação para avaliar se o seu trabalho final
>   conseguiu ou não reproduzir a referência.

- **Timestamp inicial**: <mark>`0:30`</mark>
- **Timestamp final**: <mark>`1:00`</mark>


### Imagens

> [!IMPORTANT]
> - Coloque aqui **três imagens** capturadas do vídeo acima, que você
>   irá usar como ilustração para as explicações que vêm abaixo.

<mark>`<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/0cf31e8f-db2e-4eed-b088-f618f80fa913" />
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/bd528c0e-c19e-402a-809c-ccf2045045ba" />
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/1afe6c09-571c-4d3b-9336-3a38add01f10" />
`</mark>



## Especificação textual

Para cada um dos requisitos abaixo (detalhados no [Enunciado do Trabalho final - Moodle](https://moodle.ufrgs.br/mod/assign/view.php?id=6018620)), escreva um parágrafo **curto** explicando como este requisito será atendido, apontando itens específicos do vídeo/imagens que você incluiu acima que atendem estes requisitos.

### Malhas poligonais complexas
<mark>`Ao redor da pista haverão árvores para decorar o cenário, assim como o taco, as paredes da pista e a bandeira indicando onde está o buraco do nível.`</mark>

### Transformações geométricas controladas pelo usuário
<mark>`Ao realizar a jogada a posição da bola referente ao jogador irá ser transladada para outra posição, de acordo com a direção e a força da tacada.`</mark>

### Diferentes tipos de câmeras
<mark>`Após a tacada a câmera seguirá a bola, antes disso ficando fixa atrás da mesma. A câmera também alternará conforme o turno de cada jogador. `</mark>

### Instâncias de objetos
<mark>`A cada nível teremos duas bolinhas (uma para cada jogador), a bandeira indicando onde está o buraco na pista, um mapa único e, no momento da tacada, o taco do jogador aparecerá.`</mark>

### Testes de intersecção
<mark>`Haverão testes de colisão da bolinha com as paredes e/ou obstáculos da pista, bem como com a bola do outro jogador.`</mark>

### Modelos de Iluminação em todos os objetos
<mark>`Utilizaremos o modelo de iluminação de Lambert, a ser aplicado em todos os objetos relevantes do jogo.`</mark>

### Mapeamento de texturas em todos os objetos
<mark>`A grama e as paredes da pista, bem como a bolinha terão texturas customizáveis que poderão ser escolhidas pelo jogador.`</mark>

### Movimentação com curva Bézier cúbica
<mark>`A movimentação da bolinha no caso da tacada sair do chão, a trajetória dela será de acordo com uma curva de Bézier cúbica. No início do nível a câmera rotacionará ao redor dele para mostrar o design da pista para os jogadores.`</mark>

### Animações baseadas no tempo ($\Delta t$)
<mark>`Haverão animações baseadas no tempo na movimentação da bola após a tacada, bem como em caso de colisões, e no taco do jogador no momento da jogada.`</mark>

## Limitações esperadas

> [!IMPORTANT]
> - Coloque aqui uma lista de detalhes visuais ou de interação que
>   aparecem no vídeo e/ou imagens acima, mas que você **não pretende
>   implementar** ou que você **irá implementar parcialmente**.
> - Para cada item, **explique por que** não será implementado ou por
>   que será implementado parcialmente.
>
> No vídeo acima os jogadores jogam simultaneamente (sem alternância de turnos) num multiplayer online, em nosso projeto só será possível jogar localmente, num sistema de turnos. Além disso jogo usado como inspiração conta com 9 pistas e um modo de edição de mapa, enquanto nosso trabalho terá 5 pistas padrão e não contará com o modo de edição de mapa/criação de novas pistas.

<mark>`<preencher>`</mark>
