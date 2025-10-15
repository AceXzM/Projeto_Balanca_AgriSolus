import admin from "firebase-admin";
import { readFileSync } from "fs";


if (!admin.apps.length) {
  const serviceAccount = JSON.parse(
    readFileSync("Credencial JSON, caso seu firestore for privado", "utf8")
  );

  admin.initializeApp({
    credential: admin.credential.cert(serviceAccount),
  });
}

export const banco = admin.firestore();
export default admin;

