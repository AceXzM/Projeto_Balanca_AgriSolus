import express from "express";
import { WebSocketServer } from "ws";
import admin, { banco } from "./firebaseadm.js";
import {readFileSync} from "fs";
import { DateTime } from "luxon";

export function iniciarAPI() {

const websocket = new WebSocketServer({ noServer: true })
let clients: any[] = [];

const app = express();
const PORTA_API = 3000;

app.use(express.json());

app.get('/sensores', async (req, res) =>  {
    try {
    const consulta = await banco
    .collection("sensores")
    .orderBy("timestamp", "desc")
    .limit(50)
    .get();

    const dados: any [] = []
    consulta.forEach((doc) => {
    const docData = doc.data();
    dados.push ({ 
        id: doc.id, 
        temperatura: docData.temperatura ?? null,
        umidade: docData.umidade ?? null,
        peso: docData.peso ?? null,
        CO2: docData.CO2 ?? null, 
        timestamp: docData.timestamp 
        ? DateTime.fromJSDate(docData.timestamp.toDate())
            .setZone("America/Sao_Paulo")
            .toISO()
            : null
        });
    });

    res.json(dados);

    } catch (err) {
        console.error("Erro ao buscar coleção: ", err);
        res.status(500).send("Erro ao buscar os dados");
    }

});

const servidor = app.listen(PORTA_API, () => {
    console.log(`API rodando em http://localhost:${PORTA_API}`);
});

servidor.on("upgrade", (req,socket,head) => {
    websocket.handleUpgrade(req,socket,head, (websocket) =>{
        clients.push(websocket);
        websocket.on("close", () => {
            clients = clients.filter((c) => c !== websocket);
        });
    });
});

banco.collection("sensores").orderBy("timestamp", "desc").limit(1)
    .onSnapshot((consultar) => {
        consultar.docChanges().forEach((change) => {
            if (change.type == "added") {
                const Dados = { id: change.doc.id, ...change.doc.data() };
                clients.forEach((websocket) => websocket.send(JSON.stringify(Dados)));
            }
        });
    });
}
