# Laboratório Virtual

## Responsabilidade
Ambiente operacional do `CRE` para modelar, testar, observar e evidenciar estratégias de reframing contextual.

## Base de Referência
O `LV` é a referência prática do projeto. Toda evolução posterior de CLI, domínio, persistência ou UI deve respeitar esta base.

## Entidades-base
- `Caso`
  - descreve o problema real
  - registra rejeição automática
  - registra gatilho
  - define contexto atual
  - define objetivo da rodada
- `Hipótese`
  - declara o reframing a ser testado
  - explicita o que deve mudar
  - define critério de sucesso
  - liga a hipótese ao caso
- `Experimento`
  - transforma a hipótese em execução
  - define passos
  - define guardrails
  - define critérios de interrupção
  - registra status da rodada
- `Evidência`
  - captura observações
  - informa fonte
  - informa confiança
  - diz se apoia ou enfraquece a hipótese
- `Decisão`
  - fecha a rodada com `promover`, `iterar` ou `descartar`

## Convenção de IDs e versão
- Caso: `LV-CASE-<numero>`
- Hipótese: `LV-HYP-<numero>`
- Experimento: `LV-EXP-<numero>`
- Evidência: `LV-EVD-<numero>`
- Versão semântica: `vMAJOR.MINOR.PATCH`

## Fluxo Canônico
1. criar caso
2. criar hipótese
3. planejar experimento
4. executar rodada
5. registrar evidências
6. decidir
7. versionar aprendizado

## Estados mínimos
- `draft`
- `ready`
- `running`
- `review`
- `archived`

## Invariantes
- sem `Caso`, não existe `Hipótese`
- sem `Hipótese`, não existe `Experimento`
- sem `Evidência`, não existe `Decisão`
- sem limite ético explícito, a rodada não deve avançar
- sem entrega real, o reframing não pode ser promovido

## UI mínima de referência
- lista de casos
- detalhe do caso
- hipótese ativa
- experimento da rodada
- painel de evidências
- decisão final

## Fronteiras
- o laboratório não define a tese do `CRE`
- o laboratório executa e observa
- decisões conceituais continuam no núcleo do `CRE`

## Regra central
O `LV` deve sempre responder:

> Que contexto estamos mudando, qual gatilho queremos evitar, que evidência mostra que isso funcionou, e com que limite ético?
