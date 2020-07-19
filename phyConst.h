#ifndef PHY_CONST_H
# define PHY_CONST_H

// ----------------------------------------------------------
// physical constants
# define            M           = 1000000L;

static const float  CuFtPcuIn   = 1.0 / (12 * 12 * 12);
static const float  GalPcuFt    = 7.5;
static const float  LbPgal      = 8.34;
static const float  LbPkg       = 2.2;
static const int    LbPton      = 2000;
static const float  MphTfps     = 5280.0 / 3600;   // mph to fps
static const float  PsiStd      = 14.7;          // standard atmospheric pressure;
static const float  SqFtPsqM    = (39.4 / 12) * (39.4 / 12);

static const float  NbrTon      = 0.10 * LbPton; // nominal brake ratio per Ton

static const int    Ncyl        = 2;
static const int    NstrPcyc    = 2;

#endif
