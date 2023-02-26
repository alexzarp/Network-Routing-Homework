package message

type Message struct {
	Type    int    // tipo: 1 - Mensagem | 0 - Controle
	Root    string // endereço ip da fonte
	Middle  string // endereço ip do gateway mais próximo
	Destiny string // endereço ip do destino
	Size    string // tamanho da mensagem
	Payload string // conteúdo da mensagem
}
