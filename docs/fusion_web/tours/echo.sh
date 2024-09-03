#!/bin/bash

# Enviar los encabezados HTTP


# Mostrar un encabezado HTML
echo "<html>"
echo "<head><title>Contenido Dinamico</title></head>"
echo "<body>"

# Mostrar la fecha y hora actual
echo "<h1>Contenido Dinamico</h1>"
echo "<p>Fecha y hora actual: $(date)</p>"

# Mostrar parametros de consulta
echo "<h2>Parametros de Consulta:</h2>"
echo "<ul>"

# Leer y mostrar los parámetros de consulta
QUERY_STRING=$(echo "$QUERY_STRING" | sed 's/&/<br>/g')
if [ -n "$QUERY_STRING" ]; then
    IFS='&' read -r -a params <<< "$QUERY_STRING"
    for param in "${params[@]}"; do
        echo "<li>$param</li>"
    done
else
    echo "<li>No hay parametros de consulta.</li>"
fi

# Mostrar el pie de página HTML
echo "</ul>"
echo "</body>"
echo "</html>"
