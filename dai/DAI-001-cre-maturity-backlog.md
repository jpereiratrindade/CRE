# DAI-001 - CRE Maturity Backlog

## Status
Ativo

## Objetivo
Subir maturidade, confiabilidade e governança do `CRE`, conectando o motor conceitual ao `Laboratório Virtual` como sistema único.

## Itens
1. `P0` Formalizar `CRECycle` como artefato oficial do domínio
   - owner: `core`
   - status: `done`
   - saída: `ddd/CRECycle.md`

2. `P0` Tornar templates do LV conscientes do ciclo CRE
   - owner: `lv`
   - status: `done`
   - saída: templates e exemplos com `Rejeição`, `Gatilho`, `Reframing` e `Exposição`

3. `P0` Conectar README e docs ao posicionamento `CRE -> LV`
   - owner: `docs`
   - status: `done`
   - saída: README e DDD atualizados

4. `P1` Introduzir `PerceptionShiftReport` no código
   - owner: `core`
   - status: `done`
   - saída: tipo explícito no domínio C++

5. `P1` Representar `Rejeição`, `Gatilho`, `Reframing` e `Exposição` como entidades/VOs no core
   - owner: `core`
   - status: `done`
   - saída: modelagem C++ alinhada ao `CRECycle`

6. `P1` Fazer a GUI navegar múltiplas rodadas do workspace
   - owner: `ui`
   - status: `done`
   - saída: seletor de rodada + histórico

7. `P1` Criar backlog DAI operacional por rodada
   - owner: `ops`
   - status: `todo`
   - saída: convenção para decisões, riscos, donos e próximos passos

8. `P2` Adicionar leak check e guardas extras de execução no CI
   - owner: `quality`
   - status: `in_progress`
   - saída: mais confiabilidade de execução

9. `P2` Produzir casos reais adicionais
   - owner: `research`
   - status: `todo`
   - saída: ao menos educação + STRATA

## Critério de sucesso
- `CRE` reconhecido como pipeline dominante do sistema
- `LV` executando ciclos CRE explícitos
- artefatos versionados com rastreabilidade de decisão
- avanço mensurável nas notas de maturidade e confiabilidade do `LabGestao`
