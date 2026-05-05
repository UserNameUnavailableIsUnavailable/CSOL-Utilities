ifndef PROXY_MK
PROXY_MK := 1

PROXY ?=
export NO_PROXY ?= "localhost,::1"
export HTTP_PROXY ?= $(PROXY)
export HTTPS_PROXY ?= $(PROXY)

endif # !PROXY_MK
