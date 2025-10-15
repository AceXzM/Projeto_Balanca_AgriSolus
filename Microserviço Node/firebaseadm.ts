import admin from "firebase-admin";
import { readFileSync } from "fs";


if (!admin.apps.length) {
  const serviceAccount = JSON.parse(
    readFileSync("./src/teste-bc360-firebase-adminsdk-fbsvc-f0a09b9b97.json", "utf8")
  );

  admin.initializeApp({
    credential: admin.credential.cert(serviceAccount),
  });
}

export const banco = admin.firestore();
export default admin;
