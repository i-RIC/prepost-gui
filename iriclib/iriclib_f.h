! Fortran version of iriclib.h

      integer iRIC_GEO_UNKNOWN, IRIC_GEO_POLYGON, &
        IRIC_GEO_RIVERSURVEY, IRIC_GEO_POINTMAP
      parameter(IRIC_GEO_UNKNOWN     = 0)
      parameter(IRIC_GEO_POLYGON     = 1)
      parameter(IRIC_GEO_RIVERSURVEY = 2)
      parameter(IRIC_GEO_POINTMAP    = 3)

      integer IRIC_OPTION_UNKNOWN, IRIC_OPTION_CANCEL
      parameter(IRIC_OPTION_UNKNOWN  = 0)
      parameter(IRIC_OPTION_CANCEL   = 1)

      integer IRIC_LOCKED
      parameter(IRIC_LOCKED          = 1)

      integer IRIC_CANCELED
      parameter(IRIC_CANCELED        = 1)
