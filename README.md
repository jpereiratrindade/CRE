# CRE

Context Reframing Engine.

O `CRE` e o motor de redefinicao de contexto para estudar, modelar e validar estrategias de reframing diante de rejeicao automatica ou preconceitos de percepcao.

O primeiro ambiente operacional do projeto e o `Laboratorio Virtual`, onde casos sao simulados, testados e evidenciados com governanca explicita.

## Fundacao Conceitual
- O ADR fundador esta em `adr/ADR-AGENT-005-cre-foundation-virtual-lab.md`
- O nucleo do motor esta descrito em `ddd/ContextReframingEngine.md`
- O primeiro ambiente operacional esta descrito em `ddd/LaboratorioVirtual.md`
- Os limites eticos iniciais estao em `policies/ethics_limits.md`

## Bootstrap
- Configurar build:
  - `cmake -S . -B build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON`
- Compilar:
  - `cmake --build build`
- Testar:
  - `ctest --test-dir build --output-on-failure`
- Quality local:
  - `./scripts/run_quality.sh`
- Validar governança:
  - `python3 scripts/validate_tool_contracts.py`
  - `python3 scripts/validate_governance_repo.py`
- Instalar hooks:
  - `./scripts/dev/install-git-hooks.sh`

## Estrutura
- `src/`: implementação
- `include/`: headers públicos
- `tests/`: testes automatizados
- `adr/`: decisões arquiteturais
- `ddd/`: modelagem de dominio do CRE e do Laboratorio Virtual
- `policies/`: regras operacionais
- `mcp/`: contratos executáveis para ferramentas/agentes
- `prompts/`: contexto estruturado
- `scripts/`: automações locais
- `.github/workflows/`: CI

## Conceitos centrais
- `RejeicaoAutomatica`: filtro mental ou heuristica que bloqueia a percepcao inicial
- `Gatilho`: elemento que ativa a rejeicao
- `Contexto`: moldura de apresentacao e sequenciamento
- `Experiencia`: contato real com o valor entregue
- `Reforco`: consolidacao posterior da nova percepcao

## Ambientes
- `CRE`: motor conceitual e operacional
- `Laboratorio Virtual`: ambiente de simulacao, teste e validacao de reframing contextual

## Pipeline Inicial
1. Mapear a rejeicao automatica
2. Identificar o gatilho de ativacao
3. Redesenhar o contexto de entrada
4. Garantir entrega real
5. Criar oportunidade de teste
6. Registrar evidencia e reforco posterior

## Principios
- Reframing nao substitui substancia
- Explicacao excessiva nao e estrategia
- O laboratorio existe para evidenciar, nao para maquiar
- Toda conclusao relevante precisa de hipotese, experimento e evidencia

## Proximo Passo Natural
- transformar o Laboratorio Virtual em um modulo mais operacional com casos, hipoteses, experimentos e evidencias versionadas
