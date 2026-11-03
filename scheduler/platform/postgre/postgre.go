package postgre

import (
	"bytes"
	"context"
	"text/template"
	"time"

	"git.macrohard.fun/root/csol-utilities/Scheduler/config"
	"github.com/jackc/pgx/v5/pgxpool"
)

func CreateDSN(dbCfg *config.DB) (string, error) {
	tmpl, err := template.New("dsn").Parse("postgres://{{.User}}:{{.Password}}@{{.Host}}:{{.Port}}/{{.Name}}?sslmode={{.SSLMode}}")
	if err != nil {
		return "", err
	}
	var buf bytes.Buffer
	err = tmpl.Execute(&buf, dbCfg)
	return buf.String(), err
}

func New(dbCfg *config.DB) (*pgxpool.Pool, error) {

	dsn, err := CreateDSN(dbCfg)
	if err != nil {
		return nil, err
	}

	ctx, cancel := context.WithTimeout(context.Background(), 5 * time.Second)
	defer cancel()

	db, err := pgxpool.New(ctx, dsn)
	if err != nil {
		return nil, err
	}

	return db, nil
}
