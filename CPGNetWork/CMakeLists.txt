add_library(cpgnetwork STATIC
		CPG_gait.c
		callODEFunctionNSM.c
        CPGNetWork.c
        CPGNetWork_emxutil.c
        CPGNetWork_initialize.c
        CPGNetWork_rtwutil.c
        CPGNetWork_terminate.c
        fileManager.c
        fprintf.c
        ode45.c
        rt_nonfinite.c
        rtGetInf.c
        rtGetNaN.c		
		)
