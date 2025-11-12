#ifndef RENDERER_H
#define RENDERER_H

void clear_terminal();
void render_client_state(const ClientData* data);
void renderer_state_loop(int sockfd);

#endif // RENDERER_H