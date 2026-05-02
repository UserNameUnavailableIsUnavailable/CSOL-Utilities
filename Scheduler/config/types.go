package config

import (
	"time"
)

type Auth struct {
	Secret string        `yaml:"secret"`
	Expiry time.Duration `yaml:"expiry"`
}

type HTTP struct {
	Port uint16 `yaml:"port"`
}

type Routes struct {
	Profiles string `yaml:"profiles"`
	Auth   string `yaml:"auth"`
}

type DB struct {
	Host     string `yaml:"host"`
	Port     int16  `yaml:"port"`
	User     string `yaml:"user"`
	Password string `yaml:"password"`
	Name     string `yaml:"name"`
	SSLMode  string   `yaml:"ssl_mode"`
}

type Config struct {
	Auth   Auth   `yaml:"auth"`
	HTTP   HTTP   `yaml:"http"`
	Routes Routes `yaml:"routes"`
	DB     DB     `yaml:"db"`
}
