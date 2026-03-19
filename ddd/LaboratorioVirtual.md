# Laboratorio Virtual

## Responsabilidade
Ambiente de experimentacao do CRE para simular cenarios, aplicar reframing e observar resultados.

## Escopo
- Registrar casos de uso
- Modelar cenarios com rejeicao automatica
- Testar sequencias de contexto
- Coletar evidencias antes e depois
- Comparar hipoteses de entrada

## Entidades de apoio
- `Cenario`: caso descrito com contexto, objetivo e risco
- `Hipotese`: explicacao testavel sobre o efeito do reframing
- `Experimento`: execucao documentada de uma estrategia
- `Evidencia`: observacao coletada durante ou apos o experimento
- `LimiteEtico`: regra que impede manipulacao sem substancia

## Convencao de IDs e versao
- Caso: `LV-CASE-<numero>`
- Hipotese: `LV-HYP-<numero>`
- Experimento: `LV-EXP-<numero>`
- Evidencia: `LV-EVD-<numero>`
- Versao semantica: `vMAJOR.MINOR.PATCH`

## Ciclo de vida dos artefatos
- `draft`: artefato incompleto, nao entra em decisao
- `ready`: artefato apto para uso na rodada
- `archived`: artefato mantido apenas para historico

## Fluxo operacional simples
1. Criar `Caso` com contexto minimo, rejeicao observada e risco.
2. Declarar `Hipotese` testavel ligada ao caso.
3. Planejar `Experimento` com passos, guardrails e criterio de interrupcao.
4. Executar experimento e registrar dados brutos.
5. Gerar `Evidencias` com classificacao de confianca.
6. Consolidar decisao da rodada: `promover|iterar|descartar`.
7. Versionar artefatos alterados e registrar rastreabilidade entre IDs.

## Fronteiras
- O laboratorio nao define a tese do CRE
- O laboratorio executa e observa
- Decisoes conceituais continuam no nucleo do CRE

## Regras
- Nenhum experimento sem hipotese declarada
- Nenhum caso sem contexto minimo
- Nenhuma conclusao sem evidencia registrada
- Nenhuma estrategia sem considerar limite etico
