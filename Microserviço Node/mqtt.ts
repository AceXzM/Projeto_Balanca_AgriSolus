import mqtt from "mqtt";
import admin, { banco } from "./firebaseadm.js";
import {readFileSync} from "fs";
import { error } from "console";


export function iniciarMqtt() {

interface DadoSensor {
  topic: string;
  value: any;
  timestamp: FirebaseFirestore.FieldValue;  
}

const client = mqtt.connect("mqtt://192.168.2.25:1883");
const buffer: DadoSensor[] = [];
const TAMANHO_BATCH = 50;
const INTERVALO = 5000;


async function mandaBatch() {
  if (buffer.length == 0) return;

  const batch = banco.batch();
  buffer.forEach(data => {
    const docRef = banco.collection("sensores").doc();
    batch.set(docRef, data);
  })

  try {
    await batch.commit();
    console.log(`Batch ${buffer.length} arquivos gravados`);
    buffer.length = 0;
  } 
  catch (err) {
    console.error("Os arquivos não foram gravados, erro: ", err);
  }
}

client.on("connect", () => {
  console.log("Conectado ao broker!");
  client.subscribe("sensor/temperatura");
});

client.on("error", (err) => {
  console.log("Erro na conexão: ", err);
}),

client.on("close", () => {
  console.log("Broker fechado");
}),

client.on("message", (topic, message) => {
  console.log(`Mensagem em ${topic}: ${message.toString()}`);

  let payload;

  try {
    payload = JSON.parse(message.toString());
  }
  catch (err) {
    console.error("Erro ao parsear: ", err);
    return;
  }

  const data = { 
  topic,
  ...payload,
  timestamp: admin.firestore.FieldValue.serverTimestamp()
  };

  buffer.push(data);

  if (buffer.length >= TAMANHO_BATCH) {
    mandaBatch();
  }
});

setInterval(mandaBatch, INTERVALO);

}
