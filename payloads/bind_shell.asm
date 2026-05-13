; open socket
; bind socket to listen to any ip
; listen to socket
; client = accept connection at socket
; dup2 client to all io
; execve bash
