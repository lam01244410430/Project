import React from 'react';
import ReactDOM from 'react-dom/client';
import './index.css';
import App from './app'; // Chú ý chữ 'a' viết thường hay hoa tuỳ tên file thật
import 'antd/dist/reset.css';

const rootElement = document.getElementById('root');

if (rootElement) {
  const root = ReactDOM.createRoot(rootElement);
  root.render(
    <React.StrictMode>
      <App />
    </React.StrictMode>
  );
} else {
  console.error("Failed to find the root element");
}