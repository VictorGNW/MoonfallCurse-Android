# ARM Mali — considerações

- GPUs Mali tipicamente usam renderização tile-based: investigar load/store de attachments, overdraw e bandwidth, sem pressupor ganho.
- Comparar precisão de shaders, registradores, formatos e layout de render passes.
- Evitar sincronizações, blits e resolve redundantes.
- Medir SGSR EdgeDir: implementação de referência Adreno não é garantia de desempenho Mali.
- Preservar caminhos rápidos em Adreno/Xclipse e não impor limites globais pela Mali-G615 MC2.
- Contadores GPU com Arm Performance Studio, quando disponíveis.
