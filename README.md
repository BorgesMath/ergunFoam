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


![Continuidade](DocsAuxiliares/CONTINUIDADE.svg)

**Momentum:**

![Continuidade](DocsAuxiliares/Ergun.svg)


![Coeficientes](DocsAuxiliares/Coeficientes.svg)


## 🔹 Notação

| Símbolo        | Significado                                                                 |
|----------------|-----------------------------------------------------------------------------|
| $\mathbf{U}$   | Vetor velocidade                                                            |
| $p$            | Pressão cinemática ($p/\\rho$)                                              |
| $\\nu$         | Viscosidade cinemática                                                      |
| $\\alpha$      | Coeficiente linear de resistência (termo proporcional à velocidade)         |
| $\\beta$       | Coeficiente do termo quadrático de Ergun (resistência proporcional a $|U|U$)|







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

```bash
cd $FOAM_USER_APPBIN/..path../ergunFoam # ou para onde você tenha o repositório
wclean && wmake
```

---

## Como rodar (Alterações dem um case padrão )

Adicione o arquivo **`porosityProperties`** dentro da pasta `constant` do case. Esse arquivo descreve a região porosa usada pelo solver (ex.: modelo de Ergun).

---

### Exemplo de arquivo (`case/constant/porosityProperties`)
```text
/*--------------------------------*- C++ -*----------------------------------*\
| =========                 |                                                 |
| \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox           |
|  \\    /   O peration     | Version:  v2406                                 |
|   \\  /    A nd           | Website:  www.openfoam.com                      |
|    \/     M anipulation  |                                                 |
\*---------------------------------------------------------------------------*/
FoamFile
{
    version     2.0;
    format      ascii;
    class       dictionary;
    location    "constant";
    object      porosityProperties;
}

// --------------------------------------------------------------- //

porousRegion
{
    type              ergun;          // Modelo de porosidade (Ergun)

    // Propriedades da região porosa:
    epsilon           0.55;           // Porosidade (adimensional, 0 < epsilon < 1)
    particleDiameter  1.12e-4;        // Diâmetro da partícula (m)
}

// ************************************************************************* //


---


