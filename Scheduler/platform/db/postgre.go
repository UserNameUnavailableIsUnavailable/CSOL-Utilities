package db

import (
	"bytes"
	"context"
	"database/sql"
	"text/template"
	"time"
	_ "github.com/jackc/pgx/v4/stdlib"

	"git.macrohard.fun/root/csol-utilities/Scheduler/config"
)

func CreateDSN(dbCfg *config.DB) (string, error) {
	tmpl, err := template.New("dsn").Parse("host={{.Host}} port={{.Port}} user={{.User}} password={{.Password}} dbname={{.Name}} sslmode={{if .SSLMode}}enable{{else}}disable{{end}}")
	if err != nil {
		return "", err
	}
	var buf bytes.Buffer
	err = tmpl.Execute(&buf, dbCfg)
	return buf.String(), err
}

func NewPostgre(dbCfg *config.DB) (*sql.DB, error) {

	dsn, err := CreateDSN(dbCfg)
	if err != nil {
		return nil, err
	}

	db, err := sql.Open("pgx", dsn)
	if err != nil {
		return nil, err
	}

	ctx, cancel := context.WithTimeout(context.Background(), 5 * time.Second)
	defer cancel()
	
	if err := db.PingContext(ctx); err != nil {
		return nil, err
	}

	return db, nil
}
