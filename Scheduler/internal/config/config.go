package config

import (
	"os"
	"time"

	"gopkg.in/yaml.v3"
)

// Config holds all runtime configuration for the server.
type Config struct {
	HTTP HTTPConfig `yaml:"http"`
	GRPC GRPCConfig `yaml:"grpc"`
	JWT  JWTConfig  `yaml:"jwt"`
	DB   DBConfig   `yaml:"db"`
}

type HTTPConfig struct {
	Addr string `yaml:"addr"` // e.g. ":8080"
}

type GRPCConfig struct {
	Addr string `yaml:"addr"` // e.g. ":9090"
}

type JWTConfig struct {
	Secret string        `yaml:"secret"`
	Expiry time.Duration `yaml:"expiry"` // e.g. "24h"
}

type DBConfig struct {
	PostgresDSN string `yaml:"postgres_dsn"`
	RedisAddr   string `yaml:"redis_addr"`
}

// Load reads a YAML file and returns the parsed Config.
func Load(path string) (*Config, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, err
	}

	cfg := &Config{
		HTTP: HTTPConfig{Addr: ":8080"},
		GRPC: GRPCConfig{Addr: ":9090"},
		JWT:  JWTConfig{Expiry: 24 * time.Hour},
	}
	if err := yaml.Unmarshal(data, cfg); err != nil {
		return nil, err
	}
	return cfg, nil
}
