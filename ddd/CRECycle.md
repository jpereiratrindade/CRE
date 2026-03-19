# CRE Cycle

## Responsabilidade
Operacionalizar o `Context Reframing Engine` como pipeline dominante do sistema, conectando percepção, experimento e decisão em um único fluxo observável.

## Propósito
Transformar rejeição automática em objeto explícito de investigação, para que o `Laboratório Virtual` deixe de executar rodadas genéricas e passe a executar rodadas orientadas por reframing contextual.

## Etapas do ciclo
1. `Rejeição`
   - identificar a heurística latente que bloqueia a avaliação racional
   - registrar a frase, o sinal ou o comportamento associado
2. `Gatilho`
   - identificar o elemento que ativa a rejeição
   - linguagem, estética, sequência, ambiente, naming ou associação simbólica
3. `Reframing`
   - desenhar o contexto alternativo de entrada
   - explicitar como a percepção inicial deve ser deslocada
4. `Exposição`
   - aplicar o novo contexto em condição controlada
   - garantir que a entrega real apareça antes da sobrecarga conceitual
5. `Evidência`
   - registrar o que mudou, o que não mudou e com que confiança
   - diferenciar observação, métrica, transcrição e artefato
6. `Decisão`
   - concluir `promover`, `iterar` ou `descartar`
   - justificar a decisão por evidência, não por intuição

## Saída principal
`PerceptionShiftReport`

## Conteúdo mínimo do relatório
- `case_id`
- `latent_rejection`
- `activation_trigger`
- `reframing_strategy`
- `exposure_design`
- `evidence_summary`
- `ethical_limit`
- `decision`
- `next_step`

## Relação com o Laboratório Virtual
O `Laboratório Virtual` executa o `CRE Cycle`.
Isso implica que:
- `Caso` deve carregar `Rejeição` e `Gatilho`
- `Hipótese` deve declarar `Reframing` esperado
- `Experimento` deve descrever a `Exposição`
- `Evidência` deve medir `Mudança de percepção`
- `Decisão` deve fechar o ciclo com base no deslocamento observado

## Invariantes
- sem `Rejeição` explícita, não há ciclo CRE válido
- sem `Gatilho` identificado, o reframing é cego
- sem `Entrega real`, o reframing não pode ser promovido
- sem `Evidência`, a decisão é inválida
- sem `Limite ético`, o ciclo não deve avançar para execução

## Pergunta central
> Que rejeição estamos tentando desativar, o que a ativa, qual contexto alternativo foi desenhado, que evidência mostra mudança de percepção, e o que decidimos fazer a seguir?
