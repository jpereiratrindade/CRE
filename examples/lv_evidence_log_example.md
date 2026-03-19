# LV Evidence Log Example

## Registro consolidado

| evidence_id | versao | experimento | tipo | confianca | uso em conclusao |
| --- | --- | --- | --- | --- | --- |
| LV-EVD-101 | v1.0.0 | LV-EXP-101 | observacao | medio | sim |
| LV-EVD-102 | v1.0.0 | LV-EXP-101 | transcricao | alto | sim |
| LV-EVD-103 | v0.1.0 | LV-EXP-102 | metrica | baixo | nao |

## Regras aplicadas
- evidencia com confianca baixa nao fecha conclusao isoladamente
- versao antiga pode ser mantida apenas para rastreabilidade
- toda conclusao referencia pelo menos 2 evidencias
