Info<< "Reading transportProperties\n" << endl;

IOdictionary transportProperties
(
    IOobject
    (
        "transportProperties",
        runTime.constant(),
        mesh,
        IOobject::MUST_READ_IF_MODIFIED,
        IOobject::NO_WRITE
    )
);


// Leitura da viscosidade cinemática (nu) e densidade (rho)
dimensionedScalar nu("nu", dimViscosity, transportProperties);
dimensionedScalar rho("rho", dimDensity, transportProperties);

dimensionedScalar mu("mu", nu.dimensions()*rho.dimensions(), nu.value()*rho.value());


/*===========================================================================*/

// Leitura do dicionário porosityProperties e sub-dicionário da região porosa
IOdictionary porousDict
(
    IOobject
    (
        "porosityProperties",
        runTime.constant(),
        mesh,
        IOobject::MUST_READ,
        IOobject::NO_WRITE
    )
);
dictionary regionDict = porousDict.subDict("porousRegion"); // ajuste para o nome correto da região

// Extrai epsilon e dp diretamente como scalar usando get<>
scalar epsilon = regionDict.get<scalar>("epsilon");
scalar dp = regionDict.get<scalar>("particleDiameter");


// Cálculo dos coeficientes de Ergun
// α = 150·(1−ε)²·μ / (ε³·dp²)
// β = 1.75·(1−ε) / (ε³·dp)

dimensionedScalar alpha
(
    "alpha",
    dimensionSet(0, 0, -1, 0, 0),  // Unidade correta: [1/s]
    150.0 * pow(1.0 - epsilon, 2) * nu.value() / (pow(epsilon, 3) * pow(dp, 2))
);


dimensionedScalar beta
(
    "beta",
    dimensionSet(0, -1, 0, 0, 0),  // Unidade correta: [1/m]
    1.75 * (1.0 - epsilon) / (pow(epsilon, 3) * dp)
);

/*===========================================================================*/

Info<< "Reading field p\n" << endl;
volScalarField p
(
    IOobject
    (
        "p",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);


Info<< "Reading field U\n" << endl;
volVectorField U
(
    IOobject
    (
        "U",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);


#include "createPhi.H"


label pRefCell = 0;
scalar pRefValue = 0.0;
setRefCell(p, mesh.solutionDict().subDict("PISO"), pRefCell, pRefValue);
mesh.setFluxRequired(p.name());
