.PHONY: run build proto tidy

# Download / tidy module dependencies
tidy:
	go mod tidy

# Build the server binary
build: tidy
	go build -o bin/server.exe .

# Run the server locally
run: tidy
	go run .

# Regenerate protobuf Go code (requires protoc, protoc-gen-go, protoc-gen-go-grpc)
proto:
	protoc --go_out=internal/rpc/proto --go_opt=paths=source_relative \
	       --go-grpc_out=internal/rpc/proto --go-grpc_opt=paths=source_relative \
	       proto/csol.proto
