// Trigger the hidden file input
function triggerFileInput() {
  document.getElementById('imageInput').click();
}

async function handleFileSelect(event) {
  const file = event.target.files[0];
  if (file) {
    // Prepare the file for upload
    const formData = new FormData();
    formData.append('file', file);
    
    try {
      const response = await fetch('/upload', {
        method: 'POST',
        body: formData
      });
      const data = await response.json();
      if (response.ok) {
        // Use the processed image URL returned from the server
        const processedUrl = data.processed_url;
        const itemName = prompt("Enter the name of the clothing item:");
        const categorySelect = document.getElementById("addCategorySelect");
        const styleSelect = document.getElementById("styleSelect");
        const category = categorySelect.value;
        const style = styleSelect.value;
        
        if (itemName && category && style) {
          addCloth(itemName, processedUrl, category, style);
        } else {
          alert("Please provide a name, select a category, and choose a style.");
        }
      } else {
        alert("Error uploading file: " + data.error);
      }
    } catch (error) {
      console.error("Upload error", error);
      alert("Error uploading file");
    }
  }
}

function addDeleteButton(card) {
  const deleteBtn = document.createElement("button");
  deleteBtn.className = "delete-btn";
  deleteBtn.textContent = "Delete";
  // Stop propagation so that clicking the button doesn't trigger enlargement
  deleteBtn.addEventListener("click", function(e) {
    e.stopPropagation();
    if (confirm("Do you want to delete this clothing item?")) {
      deleteCard(card);
    }
  });
  card.appendChild(deleteBtn);
}

function attachEnlargeListener(card) {
  card.addEventListener("click", function() {
    card.classList.toggle("enlarged");
  });
}

function deleteCard(card) {
  card.parentElement.removeChild(card);
}

function addCloth(name, imageUrl, category, style) {
  category = category.trim();
  category = category.charAt(0).toUpperCase() + category.slice(1);

  const mainContainer = document.getElementById("clothesContainer");
  let categoryContainer = document.getElementById("category-" + category);
  
  if (!categoryContainer) {
    categoryContainer = document.createElement("div");
    categoryContainer.id = "category-" + category;
    categoryContainer.className = "category-container";
    const header = document.createElement("h2");
    header.textContent = category;
    categoryContainer.appendChild(header);
    mainContainer.appendChild(categoryContainer);
  }

  const card = document.createElement("div");
  card.className = "cloth-card";

  const img = document.createElement("img");
  img.src = imageUrl;
  img.alt = name;

  const span = document.createElement("span");
  span.textContent = name;

  const styleInfo = document.createElement("small");
  styleInfo.textContent = "Style: " + style;

  card.appendChild(img);
  card.appendChild(span);
  card.appendChild(styleInfo);

  // Add delete button and enlarge event listener
  addDeleteButton(card);
  attachEnlargeListener(card);

  categoryContainer.appendChild(card);
}

// For preloaded cards, add features if missing
function addFeaturesToPreloadedCards() {
  const cards = document.querySelectorAll('.cloth-card');
  cards.forEach(card => {
    if (!card.querySelector('.delete-btn')) {
      addDeleteButton(card);
    }
    if (!card.classList.contains('enlarge-listener-attached')) {
      attachEnlargeListener(card);
      card.classList.add('enlarge-listener-attached');
    }
  });
}

function filterCategories() {
  const filterSelect = document.getElementById("categoryFilter");
  const chosenCategory = filterSelect.value;
  
  const allContainers = document.querySelectorAll('.category-container');
  if (chosenCategory === "All") {
    allContainers.forEach(container => container.style.display = "block");
  } else {
    allContainers.forEach(container => {
      container.style.display = (container.id === "category-" + chosenCategory) ? "block" : "none";
    });
  }
}

document.addEventListener("DOMContentLoaded", function() {
  addFeaturesToPreloadedCards();
  document.getElementById("categoryFilter").addEventListener("change", filterCategories);
});
