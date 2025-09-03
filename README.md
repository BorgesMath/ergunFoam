# ergunFoam

## Application
`ergunFoam`

## Group
`grpIncompressible PorousMedia Solvers`

**Baseado em:**
- `icoFoam` — guia e referência do solver `icoFoam` (incompressible transient solver). Consulte a documentação oficial: https://www.openfoam.com/documentation/guides/latest/doc/guide-applications-solvers-incompressible-icoFoam.html

## Description

## Visão geral

`ergunFoam` é um solver transient (PISO) para resolver o escoamento incompressível e laminar através de meios porosos. O solver monta a equação de momento incluindo termos do tipo **Ergun** (termo linear em `U` e termo quadraticamente dependente de `|U|` linearizado), e resolve o acoplamento pressão/velocidade com o algoritmo PISO.

Este repositório contém os seguintes arquivos para o solver:

```
Make
createFields.H
ergunFoam.C
```

> O `ergunFoam.C` contém a montagem das equações, o acoplamento PISO e a implementação dos termos de resistência (Ergun). O arquivo `createFields.H` contem a declaração/instanciação dos campos necessários (U, p, parâmetros porosos etc.).

---

## Equações resolvidas

**Continuidade (incompressível):**

\[
\nabla \cdot \mathbf{U} = 0
\]

**Momentum (forma resolvida no código — notação simplificada):**

\[
\frac{\partial \mathbf{U}}{\partial t} + \nabla\cdot(\mathbf{U}\mathbf{U}) - \nabla\cdot(\nu\nabla\mathbf{U}) + \alpha\,\mathbf{U} + \beta\,|\mathbf{U}|\,\mathbf{U} = -\nabla p
\]

No código:
- `alpha` corresponde ao termo linear (implícito) — `fvm::Sp(alpha, U)`.
- `beta` multiplica `mag(Uprev)` para linearizar o termo quadrático (Ergun) — `fvm::Sp(beta * mag(Uprev), U)`.

---

## Arquivos importantes

- **ergunFoam.C** — implementação principal do solver, loop de tempo, montagem de `UEqn`, PISO, atualizações e escrita de resultados.
- **createFields.H** — criação e inicialização dos campos (U, p, propriedades físicas e porosas). Deve expor `alpha`, `beta`, `nu`, `phi`, `pRefCell`, `pRefValue` e demais campos usados no solver.
- **Make** — arquivos de configuração necessários para compilar o solver com `wmake`.

---

## Requisitos

- OpenFOAM (versão compatível com a sua base de código). Testado em versões modernas (coloque a sua versão do OpenFOAM se necessário).
- Ambiente `wmake` configurado (variáveis de ambiente do OpenFOAM carregadas).

---

## Como compilar

1. Posicione-se na pasta do solver (a pasta que contém `Make` e `ergunFoam.C`).

```bash
cd $FOAM_USER_APPBIN/..path../ergunFoam # ou para onde você tenha o repositório
wmake
```

2. Se o `wmake` responder sem erros, o binário `ergunFoam` ficará disponível na sua árvore de compilação (ou em `$FOAM_USER_APPBIN` dependendo da organização dos seus caminhos).

> Dica: se usar `wmake -j N` a compilação pode usar N núcleos; caso tenha problemas com includes, verifique se as variáveis de ambiente do OpenFOAM estão carregadas (`source /opt/openfoam*/etc/bashrc` ou similar).

---

## Como rodar (exemplo rápido)

1. Prepare um case do OpenFOAM (estrutura típica: `0/`, `constant/`, `system/`).
2. No `0/` defina pelo menos os campos `U` e `p` com BCs coerentes.
3. No diretório `constant/` coloque propriedades físicas (cinemática `nu`) e as propriedades porosas (ou variables `alpha`, `beta`) que o `createFields.H` espera.
4. Execute:

```bash
ergunFoam
```

5. Visualize resultados com `paraFoam` ou `foamToVTK`.

---

## Campos / Dicionários esperados

O solver assume (pelo código apresentado) que os seguintes campos/variáveis existem ou são criados por `createFields.H`:

- `U` — velocidade volumétrica (volVectorField)
- `p` — pressão cinemática (volScalarField)
- `nu` — viscosidade cinemática (scalard or volScalarField)
- `phi` — fluxo de massa de faces (surfaceScalarField)
- `alpha` — coeficiente linear de resistência (volScalarField)
- `beta` — coeficiente do termo quadrático (volScalarField)
- `Uprev` — campo auxiliar criado no código para linearização
- `pRefCell`, `pRefValue` — referência para a equação de pressão

> **Importante:** verifique o `createFields.H` do seu repositório para confirmar como `alpha` e `beta` são lidos (por ex. a partir de `constant/transportProperties` ou outro dicionário customizado).

---

## Observações sobre implementação

- O termo quadrático de Ergun é linearizado usando `mag(Uprev)` e, portanto, o campo `Uprev` é atualizado a cada iteração temporal; isto melhora a robustez da solução ao lidar com a não-linearidade do termo de resistência.
- O solver utiliza o laço PISO com corretores não ortogonais e `rAU = 1/UEqn.A()` para montar a equação de pressão.
- Certifique-se que as condições de contorno de pressão e velocidade são consistentes para evitar `divergence` ou erros de fluxo.

---

## Testes e validação sugeridos

- Caso simples: fluxo unidimensional por um meio poroso com condição de pressão fixas e comparativo com lei de Darcy/Ergun analítica.
- Verifique conservação de massa (`checkMesh` + `continuityErrs.H` já incluído no loop do solver).
- Varie `alpha` e `beta` para observar a transição entre regime linear (Darcy) e regime com efeitos Ergun.

---

## Troubleshooting rápido

- **Erro de compilação:** verifique includes e paths (variáveis de ambiente do OpenFOAM). Rode `wmake` na pasta correta.
- **Divergência numérica:** reduza o `deltaT`, revise esquemas em `system/fvSchemes` e tolerâncias em `system/fvSolution`.
- **Fluxo inconsistente:** cheque BCs em `0/` e as atualizações de `phi` (adjustPhi é chamado no código para manter consistência).

---

## Licença

O cabeçalho do fonte indica uso da **GNU General Public License (GPL)** — preserve o cabeçalho e os avisos de copyright ao redistribuir.

---

## Contribuições

1. Abra uma _issue_ descrevendo o caso de teste ou bug.
2. Faça um fork do repositório, crie uma branch, faça suas modificações e envie um PR.

---

## Autor

- Matheus (repositório: `ergunFoam`) — código base: `ergunFoam.C`.

---

## Referências / leitura adicional

- Documentação do OpenFOAM (PISO, discretizações fv)
- Artigos sobre a equação de Ergun e modelos de escoamento em meios porosos

---

> Se quiser, eu transformo este README em `README.md` real no seu repositório, adiciono um `exampleCase/` mínimo, ou gero um `CMake`/workflow do GitHub Actions para compilar automaticamente com `wmake`. Quer que eu faça algum desses passos agora?

