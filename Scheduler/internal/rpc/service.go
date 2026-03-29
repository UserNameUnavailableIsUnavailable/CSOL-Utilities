package rpc

import (
	"context"
	"io"
	"log/slog"

	"git.macrohard.fun/root/csol-utilities/server/internal/hub"
	pb "git.macrohard.fun/root/csol-utilities/server/internal/rpc/proto"
	"google.golang.org/grpc"
)

// controlServer implements the gRPC ControlService.
type controlServer struct {
	pb.UnimplementedControlServiceServer
	hub *hub.Hub
}

// Register installs gRPC services on the given server.
func Register(s *grpc.Server, h *hub.Hub) {
	pb.RegisterControlServiceServer(s, &controlServer{hub: h})
}

// SendState receives a stream of state updates from the C++ Controller and
// broadcasts each one to WebSocket clients (Manipulators).
func (s *controlServer) SendState(stream grpc.ClientStreamingServer[pb.ControllerState, pb.Ack]) error {
	for {
		state, err := stream.Recv()
		if err == io.EOF {
			return stream.SendAndClose(&pb.Ack{Ok: true, Message: "stream ended"})
		}
		if err != nil {
			slog.Error("SendState recv error", "error", err)
			return err
		}

		// Broadcast state to all WebSocket clients
		s.hub.Broadcast(hub.Message{
			Type:    hub.MsgFromController,
			Payload: []byte(state.StateJson),
		})
	}
}

// ExecuteCommand forwards a command from the Manipulator to the Controller.
// In this boilerplate the command is simply acknowledged; a real implementation
// would queue it for the appropriate controller gRPC stream.
func (s *controlServer) ExecuteCommand(ctx context.Context, cmd *pb.Command) (*pb.Ack, error) {
	slog.Info("ExecuteCommand", "action", cmd.Action, "payload", cmd.Payload)
	// TODO: route command to the correct controller
	return &pb.Ack{Ok: true, Message: "received"}, nil
}
