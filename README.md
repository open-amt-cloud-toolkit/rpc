# DISCONTINUATION OF PROJECT #  
This project will no longer be maintained by Intel.  
This project has been identified as having known security escapes.  
Intel has ceased development and contributions including, but not limited to, maintenance, bug fixes, new releases, or updates, to this project.  
Intel no longer accepts patches to this project.  

# Remote Provisioning Client

> Disclaimer: Production viable releases are tagged and listed under 'Releases'.  All other check-ins should be considered 'in-development' and should not be used in production

The Remote Provisioning Client (RPC) is an application that enables remote capabilities for Intel® AMT, such as as device activation and configuration. To accomplish this, RPC communicates with the Remote Provisioning Server (RPS) to activate and connect the edge device.

<br><br>

**For detailed documentation** about RPC or other features of the Open AMT Cloud Toolkit, see the [docs](https://open-amt-cloud-toolkit.github.io/docs/).

<br>

## Prerequisites

We leverage GitHub Actions as a means to build RPC automatically leveraging Github's CI/CD Infrastructure. This avoids having to deal with the challenges of getting your build environment just right on your local machine and allows you to get up and running much faster. Read more about GitHub Actions [here](https://github.blog/2019-08-08-github-actions-now-supports-ci-cd/#:~:text=GitHub%20Actions%20is%20an%20API,every%20step%20along%20the%20way.)


<p align="center">
<img src="assets/animations/forkandbuild.gif" width="650"  />
</p>

## Build the Remote Provisioning Client (RPC)

1. <a href="https://github.com/open-amt-cloud-toolkit/rpc/fork" target="_blank">Create a fork of rpc on GitHub.</a>

2. Click on the **Actions** tab at the top and select **Build RPC (Native) Debug/Release**.

3. Click the **Run Workflow** dropdown. 

4. Select the **Branch: main**, or a preferred version, from the **Use workflow from** dropdown. 

5. By default, the Build Type should be **release**.  

6. Click the **Run Workflow** button. Grab a coffee and take a break! The build time ranges from 15 to 20 minutes.

8. Once the download is complete, click the completed job which will feature a green checkmark.

9. Download the appropriate RPC for your managed device's OS under the **Artifacts** section.

### To Delete your workflow run

1. Click the ellipsis ( **...** ) menu for the workflow. 

2. Choose the **Delete workflow run** option.

For detailed documentation about RPC and using it to activate a device, see the [docs](https://open-amt-cloud-toolkit.github.io/docs/)
