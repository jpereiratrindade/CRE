# LV Cycle Example v1

## 1) Caso
- case_id: `LV-CASE-101`
- version: `v1.0.0`
- status: `ready`
- Contexto: proposta tecnica e percebida como complexa antes de qualquer teste.
- Rejeicao observada: "parece dificil de adotar".
- Gatilho suspeito: excesso de linguagem abstrata na abertura.

## 2) Hipotese
- hypothesis_id: `LV-HYP-101`
- version: `v1.0.0`
- related_case_id: `LV-CASE-101`
- Hipotese: Se a abertura iniciar por demonstracao curta de uso real, entao a rejeicao inicial reduz, porque o valor e percebido antes da carga conceitual.
- Criterio de sucesso: ao menos 60% dos observadores fazem perguntas de uso, nao de defesa.

## 3) Experimento
- experiment_id: `LV-EXP-101`
- version: `v1.0.0`
- related_hypothesis_id: `LV-HYP-101`
- Passos:
  1. Apresentar demo de 2 minutos.
  2. Explicar arquitetura apenas apos a demo.
  3. Coletar perguntas dos 10 primeiros minutos.
- Guardrail: interromper se houver promessa sem entrega observavel.

## 4) Evidencia
- evidence_id: `LV-EVD-101`
- version: `v1.0.0`
- related_experiment_id: `LV-EXP-101`
- Registro: 7 de 10 perguntas focaram em aplicacao pratica.
- Confianca: medio (amostra pequena).
- Decisao de uso: sim, com necessidade de nova rodada para aumentar confianca.

## Decisao do ciclo
- Resultado: `iterar`
- Motivo: sinal positivo inicial, mas amostra limitada.
