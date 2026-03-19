# CRE

Context Reframing Engine.

O `CRE` é o motor de redefinição de contexto para estudar, modelar e validar estratégias de reframing diante de rejeição automática ou preconceitos de percepção.

O primeiro ambiente operacional do projeto é o `Laboratório Virtual`, onde casos são simulados, testados e evidenciados com governança explícita.

## Fundação Conceitual
- O ADR fundador está em `adr/ADR-AGENT-005-cre-foundation-virtual-lab.md`
- O núcleo do motor está descrito em `ddd/ContextReframingEngine.md`
- O primeiro ambiente operacional está descrito em `ddd/LaboratorioVirtual.md`
- Os limites éticos iniciais estão em `policies/ethics_limits.md`

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

## CLI Inicial
- Rodar a demonstração do Laboratório Virtual:
  - `./build/cre`
  - ou `./build/cre demo`
- Registrar uma rodada inicial em artefatos Markdown:
  - `./build/cre record-round ./output/lv-round-001`

Esse comando gera arquivos versionáveis de:
- caso
- hipótese
- experimento
- evidência

## Estrutura
- `src/`: implementação
- `include/`: headers públicos
- `tests/`: testes automatizados
- `adr/`: decisões arquiteturais
- `ddd/`: modelagem de domínio do CRE e do Laboratório Virtual
- `policies/`: regras operacionais
- `mcp/`: contratos executáveis para ferramentas/agentes
- `prompts/`: contexto estruturado
- `scripts/`: automações locais
- `.github/workflows/`: CI

## Conceitos centrais
- `RejeicaoAutomatica`: filtro mental ou heurística que bloqueia a percepção inicial
- `Gatilho`: elemento que ativa a rejeicao
- `Contexto`: moldura de apresentação e sequenciamento
- `Experiencia`: contato real com o valor entregue
- `Reforco`: consolidação posterior da nova percepção

## Ambientes
- `CRE`: motor conceitual e operacional
- `Laboratório Virtual`: ambiente de simulação, teste e validação de reframing contextual

## Pipeline Inicial
1. Mapear a rejeição automática
2. Identificar o gatilho de ativação
3. Redesenhar o contexto de entrada
4. Garantir entrega real
5. Criar oportunidade de teste
6. Registrar evidência e reforço posterior

## Princípios
- Reframing não substitui substância
- Explicação excessiva não é estratégia
- O laboratório existe para evidenciar, não para maquiar
- Toda conclusão relevante precisa de hipótese, experimento e evidência

## Próximo Passo Natural
- transformar o Laboratório Virtual em um módulo mais operacional com casos, hipóteses, experimentos e evidências versionadas
