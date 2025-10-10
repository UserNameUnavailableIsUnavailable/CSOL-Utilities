# 通用代理设置，通过 gnumake PROXY="protocol://proxy:port" 进行设定
PROXY ?= ""
export NO_PROXY ?= "localhost,::1"
export HTTP_PROXY ?= $(PROXY)
export HTTPS_PROXY ?= $(PROXY)
