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
- Abrir o shell inicial em SDL2 + Dear ImGui:
  - `./build/cre gui`
- Abrir ajuda documental direto na GUI:
  - `F1` abre a aba `Documentos`
  - `Ctrl+1` `README`
  - `Ctrl+2` `Laboratório Virtual`
  - `Ctrl+3` `CRE Core`
  - `Ctrl+4` `ADR Fundador`
  - `Ctrl+5` `Ética`
- Registrar uma rodada inicial em artefatos Markdown:
  - `./build/cre record-round`
  - `./build/cre record-round --round-id lv-round-002`
  - `./build/cre record-round --workspace ./workspace`

Esse comando gera arquivos versionáveis de:
- caso
- hipótese
- experimento
- evidência

## Workspace
O `CRE` agora separa melhor método e operação:
- o repositório guarda código, docs, ADRs, DDD e políticas
- o `workspace/` guarda rodadas e artefatos operacionais do `Laboratório Virtual`

Resolução padrão do workspace:
1. `--workspace <path>`
2. variável de ambiente `CRE_WORKSPACE`
3. raiz contendo `cre.yaml`
4. fallback para `./workspace`

Estrutura mínima:
- `workspace/cre.yaml`
- `workspace/rounds/<round-id>/`
- `workspace/cases/`
- `workspace/hypotheses/`
- `workspace/experiments/`
- `workspace/evidences/`
- `workspace/cycles/`
- `workspace/logs/`
- `workspace/exports/`
- `workspace/.cre/state.json`

Compatibilidade:
- a GUI e o core priorizam `workspace/rounds/<round-id>`
- se não houver artefatos lá, ainda aceitam o legado `output/<round-id>`

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

## Direção Estratégica
O `CRE` não deve existir apenas como conceito paralelo ao `Laboratório Virtual`.
A direção do projeto é fazer do `CRE` o princípio organizador do sistema, enquanto o `Laboratório Virtual` atua como executor experimental desse motor.

Em termos práticos, isso significa evoluir de:
- `Hipótese -> Experimento -> Evidência`

para um ciclo explicitamente orientado a reframing:
- `Rejeição -> Gatilho -> Reframing -> Exposição -> Evidência -> Decisão`

## Referência do LV
A base oficial do `Laboratório Virtual` está em `ddd/LaboratorioVirtual.md` e define:
- entidades-base: `Caso`, `Hipótese`, `Experimento`, `Evidência`, `Decisão`
- fluxo canônico: caso -> hipótese -> experimento -> evidência -> decisão
- invariantes operacionais e éticos
- estados mínimos dos artefatos
- UI mínima de referência para futuras interfaces

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

## Licença
Este projeto é distribuído sob a licença `GNU GPL v3.0 or later`.
O texto integral está em [LICENSE](/run/media/jpereiratrindade/labeco10T/dev/cpp/cre/LICENSE).

## Próximo Passo Natural
- transformar o Laboratório Virtual em um módulo mais operacional com casos, hipóteses, experimentos e evidências versionadas
- formalizar um `CRE Cycle` como pipeline dominante do sistema
