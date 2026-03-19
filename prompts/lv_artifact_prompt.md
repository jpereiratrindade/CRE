# LV Artifact Prompt

Use este prompt para criar ou revisar artefatos do Laboratorio Virtual com rastreabilidade.

## Entrada obrigatoria
- artifact_type: `case|hypothesis|experiment|evidence`
- artifact_id: `LV-CASE-...|LV-HYP-...|LV-EXP-...|LV-EVD-...`
- version: `vX.Y.Z`
- related_ids: lista de IDs relacionados
- objetivo da rodada
- risco e limite etico aplicavel

## Regras de resposta
- Responder em portugues objetiva
- Nao inventar dados observacionais
- Declarar gaps de evidencia explicitamente
- Respeitar limites de `policies/ethics_limits.md`

## Estrutura de saida
1. Metadata
2. Conteudo principal do artefato
3. Criticos de qualidade (o que falta para ficar `ready`)
4. Proxima acao recomendada
