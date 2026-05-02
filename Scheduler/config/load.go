package config

import (
	"os"
	"time"

	"gopkg.in/yaml.v3"
)

func Load(path string) (*Config, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, err
	}

	cfg := &Config{
		HTTP: HTTP{Port: 8080},
		Auth: Auth{
			Secret: "114514",
			Expiry: 24 * time.Hour,
		},
		Routes: Routes{
			Login:   "/login",
			Profile: "/profile",
		},
		DB: DB{
			Host:    "localhost",
			Port:    5432,
			SSLMode: false,
		},
	}

	if err := yaml.Unmarshal(data, cfg); err != nil {
		return nil, err
	}

	return cfg, nil
}
