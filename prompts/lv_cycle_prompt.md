# LV Cycle Prompt

Objetivo: conduzir uma rodada operacional curta do Laboratorio Virtual.

## Entrada
- case_id e versao
- hipotese alvo
- janela de execucao
- criterio de sucesso
- riscos conhecidos

## Passos obrigatorios
1. Confirmar se o caso esta `ready`
2. Validar hipotese testavel
3. Definir experimento com guardrails
4. Executar coleta de evidencias
5. Classificar confianca da evidencia
6. Tomar decisao: `promover|iterar|descartar`

## Formato de saida
- Decisao final da rodada
- Evidencias usadas (IDs)
- Risco residual
- Proxima rodada sugerida
